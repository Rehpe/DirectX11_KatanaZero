#include "pch.h"
#include "CTileMap.h"

#include "CResourceManager.h"
#include "CTransform.h"

#include "CStructuredBuffer.h"

CTileMap::CTileMap()
	: CRenderComponent(COMPONENT_TYPE::TILEMAP)
	, m_iTileCountX(1)
	, m_iTileCountY(1)
	, m_SliceX(1)
	, m_SliceY(1)
{
	SetMesh(CResourceManager::GetInst()->FindRes<CMesh>(L"RectMesh"));
	SetMaterial(CResourceManager::GetInst()->FindRes<CMaterial>(L"TileMapMtrl"));

	// Ÿ���� ������� �������̹Ƿ� ����ȭ���۸� ����Ѵ�.
	m_Buffer = new CStructuredBuffer;
	m_Buffer->Create(sizeof(tTile), m_SliceX*m_SliceY,SB_TYPE::READ_ONLY, true);

	// Ÿ���� ������� �������̹Ƿ� ����ȭ���۸� ����Ѵ�.
	m_TileMapBuffer = new CStructuredBuffer;
	m_TileMapBuffer->Create(sizeof(tTileMapIdx), m_iTileCountX * m_iTileCountY, SB_TYPE::READ_ONLY, true);
}

CTileMap::~CTileMap()
{
	// ����ȭ���� ����
	if (nullptr != m_Buffer)
		delete m_Buffer;

	if (nullptr != m_TileMapBuffer)
		delete m_TileMapBuffer;

}

void CTileMap::finaltick()
{
}

void CTileMap::render()
{
	if (nullptr == GetMesh() || nullptr == GetMaterial())
		return;

	// Transform �� UpdateData ��û
	Transform()->UpdateData();

	// ���� ������Ʈ
	GetMaterial()->SetTexParam(TEX_0, m_TileAtlasTex);
	GetMaterial()->SetScalarParam(INT_0, &m_iTileCountX);
	GetMaterial()->SetScalarParam(INT_1, &m_iTileCountY);
	GetMaterial()->UpdateData();

	// ����ȭ���� ������Ʈ
	UpdateData();

	// ����
	GetMesh()->render();
}

void CTileMap::UpdateData()
{
	// m_vecTile ���� ������ ����ȭ���۷� ����
	m_Buffer->SetData(m_vecTile.data(), sizeof(tTile) * (UINT)m_vecTile.size());
	m_Buffer->UpdateData(20, PIPELINE_STAGE::PS_PIXEL);

	// m_TileMap ���� ������ ����ȭ���۷� ����
	m_TileMapBuffer->SetData(m_TileMap.data(), sizeof(tTileMapIdx) * (UINT)m_TileMap.size());
	m_TileMapBuffer->UpdateData(22, PIPELINE_STAGE::PS_PIXEL);


}

void CTileMap::SetTileCount(UINT _iXCount, UINT _iYCount)
{
	m_iTileCountX = _iXCount;
	m_iTileCountY = _iYCount;

	// ���� ũ�� ��������
	m_vecTile.clear();
	m_vecTile.resize(m_iTileCountX * m_iTileCountY);

	// ����ȭ���� ������� Ÿ�Ϻ��� ����� ũ�ٸ� ���� �����
	if (m_Buffer->GetElementCount() < m_vecTile.size())
	{
		m_Buffer->Create(sizeof(tTile), (UINT)m_vecTile.size(),SB_TYPE::READ_ONLY, true);
	}

	// Ÿ�� ���� �׽�Ʈ
	for (size_t i = 0; i < m_iTileCountY; ++i)
	{
		for (size_t j = 0; j < m_iTileCountX; ++j)
		{
			m_vecTile[i * m_iTileCountX + j].vLeftTop.x = m_vSliceSize.x * j;
			m_vecTile[i * m_iTileCountX + j].vLeftTop.y = m_vSliceSize.y * i;
			m_vecTile[i * m_iTileCountX + j].vSlice = m_vSliceSize;
		}
	}
	// ���� ũ�� ��������
	m_TileMap.clear();
	m_TileMap.resize(m_iTileCountX * m_iTileCountY);
	// ����ȭ���� ������� Ÿ�Ϻ��� ����� ũ�ٸ� ���� �����
	if (m_TileMapBuffer->GetElementCount() < m_TileMap.size())
	{
		m_TileMapBuffer->Create(sizeof(tTileMapIdx), (UINT)m_TileMap.size(), SB_TYPE::READ_ONLY, true);
	}

	for (int i = 0; i < m_TileMap.size(); i++)
	{
		m_TileMap[i].TileIdx = i;
	}
}

void CTileMap::SetSliceSize(int _SliceX, int _SliceY)
{
	m_SliceX = _SliceX;
	m_SliceY = _SliceY;

	Vec2 vResolution = Vec2(m_TileAtlasTex->Width(), m_TileAtlasTex->Height());

	Vec2 SlicePixel = vResolution / Vec2(m_SliceX, m_SliceY);
	m_vSliceSize = SlicePixel / vResolution;
}

void CTileMap::SaveToLevelFile(FILE* _File)
{
	// �θ� RenderComponent Save(Mesh, Mtrl)
	CRenderComponent::SaveToLevelFile(_File);

	fwrite(&m_iTileCountX, sizeof(UINT), 1, _File);
	fwrite(&m_iTileCountY, sizeof(UINT), 1, _File);
	fwrite(&m_vSliceSize, sizeof(Vec2), 1, _File);

	// Ÿ�� ������ ����
	fwrite(m_vecTile.data(), sizeof(tTile), m_vecTile.size(), _File);
}

void CTileMap::LoadFromLevelFile(FILE* _File)
{
	// �θ� RenderComponent Load (Mesh, Mtrl)
	CRenderComponent::LoadFromLevelFile(_File);

	fread(&m_iTileCountX, sizeof(UINT), 1, _File);
	fread(&m_iTileCountY, sizeof(UINT), 1, _File);
	fread(&m_vSliceSize, sizeof(Vec2), 1, _File);

	// Ÿ�� ���μ��� ������ �������ش� -> ���۰� resize �Ǹ� ������.
	SetTileCount(m_iTileCountX, m_iTileCountY);

	// Ÿ�� ������ �ε�
	fread(m_vecTile.data(), sizeof(tTile), m_iTileCountX * m_iTileCountY, _File);
}

