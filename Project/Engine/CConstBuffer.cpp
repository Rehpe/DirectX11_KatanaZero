#include "pch.h"
#include "CConstBuffer.h"

#include "CDevice.h"

CConstBuffer::CConstBuffer(UINT _iRegisterNum)
	: m_iRegisterNum(_iRegisterNum)
	, m_Desc{}
	, m_iElementCount(0)
	, m_iElementSize(0)
{
}

CConstBuffer::~CConstBuffer()
{
}

void CConstBuffer::Create(UINT _iElementSize, UINT _iElementCount)
{
	// 상수 버퍼 생성
	
	m_iElementSize = _iElementSize;
	m_iElementCount = _iElementCount;

	UINT iBufferSize = m_iElementSize * m_iElementCount;

	// 상수 버퍼는 메모리 단위가 16바이트이므로, 모듈러를 써서 16의 배수가 아닐시 assert
	assert(!(iBufferSize % 16));

	m_Desc.ByteWidth = _iElementSize * _iElementCount;
	m_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	m_Desc.Usage = D3D11_USAGE_DYNAMIC;
	m_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (FAILED(DEVICE->CreateBuffer(&m_Desc, nullptr, m_CB.GetAddressOf())))
	{
		assert(nullptr);
	}
}

void CConstBuffer::SetData(void* _pSrc, UINT _iSize)
{
	UINT size = _iSize;

	//Size 값이 Default 인자인 0으로 들어왔다면 상수버퍼 크기로 본다
	if (0 == _iSize)
	{
		size = m_iElementCount * m_iElementSize;
	}

	// Map 요청할 데이터 사이즈가 상수버퍼 사이즈보다 클 경우 assert 
	assert(!(size > m_iElementCount * m_iElementSize));		

	// Map, UnMap
	D3D11_MAPPED_SUBRESOURCE tSubRes = {};

	if (!FAILED(CONTEXT->Map(m_CB.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &tSubRes)))
	{
		memcpy(tSubRes.pData, _pSrc, size);
		CONTEXT->Unmap(m_CB.Get(), 0);
	}
}
  
void CConstBuffer::UpdateData()
{
	CONTEXT->VSSetConstantBuffers(m_iRegisterNum, 1, m_CB.GetAddressOf());
	CONTEXT->HSSetConstantBuffers(m_iRegisterNum, 1, m_CB.GetAddressOf());
	CONTEXT->DSSetConstantBuffers(m_iRegisterNum, 1, m_CB.GetAddressOf());
	CONTEXT->GSSetConstantBuffers(m_iRegisterNum, 1, m_CB.GetAddressOf());
	CONTEXT->PSSetConstantBuffers(m_iRegisterNum, 1, m_CB.GetAddressOf());
}

void CConstBuffer::UpdateData_CS()
{
	CONTEXT->CSSetConstantBuffers(m_iRegisterNum, 1, m_CB.GetAddressOf());
}

