#include "pch.h"
#include "CStructuredBuffer.h"

#include "CDevice.h"

CStructuredBuffer::CStructuredBuffer()
	: m_iElementSize(0)
	, m_iElementCount(0)
	, m_tDesc{}
{
}

CStructuredBuffer::~CStructuredBuffer()
{
}

void CStructuredBuffer::Create(UINT _iElementSize, UINT _iElementCount, SB_TYPE _Type, bool _bSysAccess, void* _pSysMem)
{
	// 만약 기존에 지정한 단위보다 많은 양의 요소가 들어오게 되면, 구조화버퍼를 다시 Create하는 식으로 덮어씌운다.
	// 기존 버퍼를 지우기 위해 Create 최상단에서 버퍼와 View를 nullptr로 초기화해준다.

	m_SB = nullptr;
	m_SRV = nullptr;
	m_UAV = nullptr;

	m_SB_CPU_Read = nullptr;
	m_SB_CPU_Write = nullptr;

	m_iElementSize = _iElementSize;
	m_iElementCount = _iElementCount;	

	m_Type = _Type;
	m_bSysAccess = _bSysAccess;

	UINT iBufferSize = m_iElementSize * _iElementCount;

	// 16바이트 단위 메모리 정렬
	assert(!(iBufferSize % 16));

	// 상수버퍼 생성

	m_tDesc.ByteWidth = iBufferSize;				// 버퍼 크기
	m_tDesc.StructureByteStride = m_iElementSize;	// 데이터 간격
	
	if (SB_TYPE::READ_ONLY == m_Type)	        // 읽기전용, 즉 t레지스터 바인딩
	{
		m_tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // 텍스처 레지스터에 바인딩할 것이므로 BindFlag는 Shader_Resource로
	}
	else if (SB_TYPE::READ_WRITE == m_Type) 	// 읽기, 쓰기 다 되는 Unordered Access Flag 추가
	{
		m_tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	}

	m_tDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;	// 추가 플래그로 해당 버퍼가 텍스처가 아닌 구조화버퍼라는 것을 전달

	// 기본 버퍼는 CPU와 데이터 전송이 불가능한 Defalut 버퍼
	m_tDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tDesc.CPUAccessFlags = 0;

	// 초기값 세팅 여부
	if (nullptr == _pSysMem)	// 초기값이 nullptr인 경우
	{
		if (FAILED(DEVICE->CreateBuffer(&m_tDesc, nullptr, m_SB.GetAddressOf())))
		{
			assert(nullptr);
		}
	}
	else						// 초기값이 입력된 경우
	{
		D3D11_SUBRESOURCE_DATA tSubData = {};
		tSubData.pSysMem = _pSysMem;

		HRESULT hr = DEVICE->CreateBuffer(&m_tDesc, &tSubData, m_SB.GetAddressOf());
		if (hr)
		{
			assert(nullptr);
		}
	}

	// 텍스처 레지스터에 바인딩하기 위한 ShaderResourceView 생성
	D3D11_SHADER_RESOURCE_VIEW_DESC	m_SRVDesc = {};

	m_SRVDesc.ViewDimension = D3D_SRV_DIMENSION_BUFFEREX;
	m_SRVDesc.BufferEx.NumElements = m_iElementCount;		// 요소가 몇 개 들어간 버퍼인지 

	if (FAILED(DEVICE->CreateShaderResourceView(m_SB.Get(), &m_SRVDesc, m_SRV.GetAddressOf())))
	{
		assert(nullptr);
	}
	// 레지스터 읽기/ 쓰기가 가능한 구조화버퍼라면 UAV를 만든다.
	if (SB_TYPE::READ_WRITE == m_Type)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC m_UABDesc = {};
		m_UABDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		m_UABDesc.Buffer.NumElements = m_iElementCount;

		if (FAILED(DEVICE->CreateUnorderedAccessView(m_SB.Get(), &m_UABDesc, m_UAV.GetAddressOf())))
		{
			assert(nullptr);
		}
	}

	// CPU Access 보조 버퍼생성(SysAccess가 true일 경우에만 생성된다) ->Gpu바인딩이 아니므로 View 생성이 필요 없음
 	if (m_bSysAccess)
	{
		m_tDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;

		// GPU -> CPU Read
		m_tDesc.Usage = D3D11_USAGE_DEFAULT;
		m_tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		if (FAILED(DEVICE->CreateBuffer(&m_tDesc, nullptr, m_SB_CPU_Read.GetAddressOf())))
		{
			assert(nullptr);
		}

		// CPU -> GPU Write
		m_tDesc.Usage = D3D11_USAGE_DYNAMIC;
		m_tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		if (FAILED(DEVICE->CreateBuffer(&m_tDesc, nullptr, m_SB_CPU_Write.GetAddressOf())))
		{
			assert(nullptr);
		}
	}
}

void CStructuredBuffer::SetData(void* _pSrc, UINT _iSize)
{
	// 이제 메인 버퍼는 오로지 GPU<->셰이더 간의 소통만 가능하므로, 
	// SetData시 Write버퍼에 맵핑시켜 데이터를 보내고, 보낸 데이터를 CopyResource하여 메인 버퍼로 받아오는 식으로 구조를 수정하겠다.
	// 이 보조버퍼와 메인 버퍼의 크기가 동일해야만 함수호출에 성공할 것이다.

	// 보낼 데이터가 없으면 return
	if (nullptr == _pSrc)
		return;

	// 사이즈가 지정되지 않았다면, 자동으로 ElementCount * ElementSize 하여 사이즈 계산 후 대입
	UINT iSize = _iSize;
	if (0 == iSize)
	{
		iSize = GetBufferSize();
	}

	// CPU -> CPU WriteBuffer
	D3D11_MAPPED_SUBRESOURCE tSub = {};
	CONTEXT->Map(m_SB_CPU_Write.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &tSub);
	memcpy(tSub.pData, _pSrc, iSize);
	CONTEXT->Unmap(m_SB_CPU_Write.Get(), 0);

	// CPU WriteBuffer -> Main Buffer
	CONTEXT->CopyResource(m_SB.Get(), m_SB_CPU_Write.Get());	// 받아올 버퍼, 복사할 원본데이터 주소
}

void CStructuredBuffer::GetData(void* _pDst)
{
	// 받아볼 데이터를 메인버퍼에서 Read버퍼에 복사한 뒤, Read버퍼를 맵언맵해서 Cpu로 가져오는 식으로 동작.
	// GetData의 경우 데이터를 전부 가져올 것이므로 데이터 크기 설정의 무의미하므로, 목적지 데이터 주소만 인자로 넣어준다.
	
	// Main Buffer -> CPU ReadBuffer
	CONTEXT->CopyResource(m_SB_CPU_Read.Get(), m_SB.Get());

	// CPU ReadBuffer -> CPU
	D3D11_MAPPED_SUBRESOURCE tSub = {};
	CONTEXT->Map(m_SB_CPU_Read.Get(), 0, D3D11_MAP::D3D11_MAP_READ, 0, &tSub);
	memcpy(_pDst, tSub.pData, m_iElementCount * m_iElementSize);
	CONTEXT->Unmap(m_SB_CPU_Read.Get(), 0);
}

void CStructuredBuffer::UpdateData(UINT _iRegisterNum, UINT _iPipeLineStage)
{
	if (PIPELINE_STAGE::PS_VERTEX & _iPipeLineStage)
	{
		CONTEXT->VSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());
	}

	if (PIPELINE_STAGE::PS_HULL & _iPipeLineStage)
	{
		CONTEXT->HSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());
	}

	if (PIPELINE_STAGE::PS_DOMAIN & _iPipeLineStage)
	{
		CONTEXT->DSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());
	}

	if (PIPELINE_STAGE::PS_GEOMETRY & _iPipeLineStage)
	{
		CONTEXT->GSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());
	}

	if (PIPELINE_STAGE::PS_PIXEL & _iPipeLineStage)
	{
		CONTEXT->PSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());
	}
}

void CStructuredBuffer::UpdateData_CS(UINT _iRegisterNum, bool _IsShaderRes)
{
	m_iRecentRegisterNum = _iRegisterNum;
	
	if (_IsShaderRes)	// CS를 사용하지만 읽기전용 (SRV에 바인딩)
	{
		CONTEXT->CSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());
	}
	else	// CS를 사용하여 읽고 쓰기가 가능한 방식(UAV에 바인딩) 
	{
		UINT i = -1;
		CONTEXT->CSSetUnorderedAccessViews(_iRegisterNum, 1, m_UAV.GetAddressOf(), &i);
	}
}

void CStructuredBuffer::Clear()
{
	// 모든 파이프라인 스테이지에 nullptr을 밀어넣어 레지스터 클리어
	ID3D11ShaderResourceView* pSRV = nullptr;
	CONTEXT->VSSetShaderResources(m_iRecentRegisterNum, 1, &pSRV);
	CONTEXT->HSSetShaderResources(m_iRecentRegisterNum, 1, &pSRV);
	CONTEXT->DSSetShaderResources(m_iRecentRegisterNum, 1, &pSRV);
	CONTEXT->GSSetShaderResources(m_iRecentRegisterNum, 1, &pSRV);
	CONTEXT->PSSetShaderResources(m_iRecentRegisterNum, 1, &pSRV);
	CONTEXT->CSSetShaderResources(m_iRecentRegisterNum, 1, &pSRV);
}

void CStructuredBuffer::Clear_CS(bool _IsShaderRes)
{
	if (_IsShaderRes)
	{
		ID3D11ShaderResourceView* pSRV = nullptr;
		CONTEXT->CSSetShaderResources(m_iRecentRegisterNum, 1, &pSRV);
	}
	else
	{
		ID3D11UnorderedAccessView* pUAV = nullptr;
		UINT i = -1;
		CONTEXT->CSSetUnorderedAccessViews(m_iRecentRegisterNum, 1, &pUAV, &i);
	}
}
