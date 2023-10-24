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
	// ���� ������ ������ �������� ���� ���� ��Ұ� ������ �Ǹ�, ����ȭ���۸� �ٽ� Create�ϴ� ������ ������.
	// ���� ���۸� ����� ���� Create �ֻ�ܿ��� ���ۿ� View�� nullptr�� �ʱ�ȭ���ش�.

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

	// 16����Ʈ ���� �޸� ����
	assert(!(iBufferSize % 16));

	// ������� ����

	m_tDesc.ByteWidth = iBufferSize;				// ���� ũ��
	m_tDesc.StructureByteStride = m_iElementSize;	// ������ ����
	
	if (SB_TYPE::READ_ONLY == m_Type)	        // �б�����, �� t�������� ���ε�
	{
		m_tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // �ؽ�ó �������Ϳ� ���ε��� ���̹Ƿ� BindFlag�� Shader_Resource��
	}
	else if (SB_TYPE::READ_WRITE == m_Type) 	// �б�, ���� �� �Ǵ� Unordered Access Flag �߰�
	{
		m_tDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	}

	m_tDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;	// �߰� �÷��׷� �ش� ���۰� �ؽ�ó�� �ƴ� ����ȭ���۶�� ���� ����

	// �⺻ ���۴� CPU�� ������ ������ �Ұ����� Defalut ����
	m_tDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tDesc.CPUAccessFlags = 0;

	// �ʱⰪ ���� ����
	if (nullptr == _pSysMem)	// �ʱⰪ�� nullptr�� ���
	{
		if (FAILED(DEVICE->CreateBuffer(&m_tDesc, nullptr, m_SB.GetAddressOf())))
		{
			assert(nullptr);
		}
	}
	else						// �ʱⰪ�� �Էµ� ���
	{
		D3D11_SUBRESOURCE_DATA tSubData = {};
		tSubData.pSysMem = _pSysMem;

		HRESULT hr = DEVICE->CreateBuffer(&m_tDesc, &tSubData, m_SB.GetAddressOf());
		if (hr)
		{
			assert(nullptr);
		}
	}

	// �ؽ�ó �������Ϳ� ���ε��ϱ� ���� ShaderResourceView ����
	D3D11_SHADER_RESOURCE_VIEW_DESC	m_SRVDesc = {};

	m_SRVDesc.ViewDimension = D3D_SRV_DIMENSION_BUFFEREX;
	m_SRVDesc.BufferEx.NumElements = m_iElementCount;		// ��Ұ� �� �� �� �������� 

	if (FAILED(DEVICE->CreateShaderResourceView(m_SB.Get(), &m_SRVDesc, m_SRV.GetAddressOf())))
	{
		assert(nullptr);
	}
	// �������� �б�/ ���Ⱑ ������ ����ȭ���۶�� UAV�� �����.
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

	// CPU Access ���� ���ۻ���(SysAccess�� true�� ��쿡�� �����ȴ�) ->Gpu���ε��� �ƴϹǷ� View ������ �ʿ� ����
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
	// ���� ���� ���۴� ������ GPU<->���̴� ���� ���븸 �����ϹǷ�, 
	// SetData�� Write���ۿ� ���ν��� �����͸� ������, ���� �����͸� CopyResource�Ͽ� ���� ���۷� �޾ƿ��� ������ ������ �����ϰڴ�.
	// �� �������ۿ� ���� ������ ũ�Ⱑ �����ؾ߸� �Լ�ȣ�⿡ ������ ���̴�.

	// ���� �����Ͱ� ������ return
	if (nullptr == _pSrc)
		return;

	// ����� �������� �ʾҴٸ�, �ڵ����� ElementCount * ElementSize �Ͽ� ������ ��� �� ����
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
	CONTEXT->CopyResource(m_SB.Get(), m_SB_CPU_Write.Get());	// �޾ƿ� ����, ������ ���������� �ּ�
}

void CStructuredBuffer::GetData(void* _pDst)
{
	// �޾ƺ� �����͸� ���ι��ۿ��� Read���ۿ� ������ ��, Read���۸� �ʾ���ؼ� Cpu�� �������� ������ ����.
	// GetData�� ��� �����͸� ���� ������ ���̹Ƿ� ������ ũ�� ������ ���ǹ��ϹǷ�, ������ ������ �ּҸ� ���ڷ� �־��ش�.
	
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
	
	if (_IsShaderRes)	// CS�� ��������� �б����� (SRV�� ���ε�)
	{
		CONTEXT->CSSetShaderResources(_iRegisterNum, 1, m_SRV.GetAddressOf());
	}
	else	// CS�� ����Ͽ� �а� ���Ⱑ ������ ���(UAV�� ���ε�) 
	{
		UINT i = -1;
		CONTEXT->CSSetUnorderedAccessViews(_iRegisterNum, 1, m_UAV.GetAddressOf(), &i);
	}
}

void CStructuredBuffer::Clear()
{
	// ��� ���������� ���������� nullptr�� �о�־� �������� Ŭ����
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
