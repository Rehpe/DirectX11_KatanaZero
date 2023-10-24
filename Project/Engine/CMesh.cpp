#include "pch.h"
#include "CMesh.h"

#include "CDevice.h"

CMesh::CMesh(bool _bEngine)
	: CResource(RES_TYPE::MESH, _bEngine)
	, m_tVBDesc{}
	, m_VtxCount(0)
	, m_tIBDesc{}
	, m_IdxCount(0)
	, m_pVtxSys(nullptr)
	, m_pIdxSys(nullptr)
{
}

CMesh::~CMesh()
{
	if (nullptr != m_pVtxSys)
		delete[] m_pVtxSys;

	if (nullptr != m_pIdxSys)
		delete[] m_pIdxSys;
}

void CMesh::Create(void* _VtxSysMem, UINT _iVtxCount, void* _IdxSysMem, UINT _IdxCount)
{
	m_VtxCount = _iVtxCount;
	m_IdxCount = _IdxCount;

	//m_Topology = _topology;

	// SystemMem 데이터 복사
	m_pVtxSys = new Vtx[m_VtxCount];
	memcpy(m_pVtxSys, _VtxSysMem, sizeof(Vtx) * m_VtxCount);

	m_pIdxSys = new UINT[m_IdxCount];
	memcpy(m_pIdxSys, _IdxSysMem, sizeof(UINT) * m_IdxCount);

	// Vertex 버퍼 생성
	m_tVBDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	m_tVBDesc.CPUAccessFlags = 0;
	m_tVBDesc.Usage = D3D11_USAGE_DEFAULT;

	m_tVBDesc.ByteWidth = sizeof(Vtx) * m_VtxCount;

	D3D11_SUBRESOURCE_DATA tSub = {};
	tSub.pSysMem = _VtxSysMem;
	if (FAILED(DEVICE->CreateBuffer(&m_tVBDesc, &tSub, m_VB.GetAddressOf())))
	{
		assert(nullptr);
	}

	// Index 버퍼 생성 
	m_tIBDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	m_tIBDesc.CPUAccessFlags = 0;
	m_tIBDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tIBDesc.ByteWidth = sizeof(UINT) * m_IdxCount;

	tSub.pSysMem = _IdxSysMem;
	if (FAILED(DEVICE->CreateBuffer(&m_tIBDesc, &tSub, m_IB.GetAddressOf())))
	{
		assert(nullptr);
	}
}

void CMesh::CreateLine(void* _VtxSysMem, UINT _iVtxCount, void* _IdxSysMem, UINT _IdxCount)
{
	m_Topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	Create(_VtxSysMem, _iVtxCount, _IdxSysMem, _IdxCount);
}

void CMesh::UpdateData()
{
	// IA 단계에서 사용할 설정
	UINT iStride = sizeof(Vtx);
	UINT iOffset = 0;
	CONTEXT->IASetVertexBuffers(0, 1, m_VB.GetAddressOf(), &iStride, &iOffset);
	CONTEXT->IASetIndexBuffer(m_IB.Get(), DXGI_FORMAT_R32_UINT, 0);

	//CONTEXT->IASetPrimitiveTopology(m_Topology);
}

void CMesh::render()
{
	UpdateData();
	CONTEXT->DrawIndexed(m_IdxCount, 0, 0);
}

void CMesh::render_particle(UINT _iParticleCount)
{
	UpdateData();

	// 인스턴싱(같은 메쉬, 재질을 사용한다면 인자로 들어온 개수만큼을 한번에 렌더링하여 드로우콜을 1로 처리한다)
	CONTEXT->DrawIndexedInstanced(m_IdxCount, _iParticleCount, 0, 0, 0);
}
