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

	// 타일의 사이즈는 가변적이므로 구조화버퍼를 사용한다.
	m_Buffer = new CStructuredBuffer;
	m_Buffer->Create(sizeof(tTile), m_SliceX*m_SliceY,SB_TYPE::READ_ONLY, true);

	// 타일의 사이즈는 가변적이므로 구조화버퍼를 사용한다.
	m_TileMapBuffer = new CStructuredBuffer;
	m_TileMapBuffer->Create(sizeof(tTileMapIdx), m_iTileCountX * m_iTileCountY, SB_TYPE::READ_ONLY, true);
}

CTileMap::~CTileMap()
{
	// 구조화버퍼 해제
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

	// Transform 에 UpdateData 요청
	Transform()->UpdateData();

	// 재질 업데이트
	GetMaterial()->SetTexParam(TEX_0, m_TileAtlasTex);
	GetMaterial()->SetScalarParam(INT_0, &m_iTileCountX);
	GetMaterial()->SetScalarParam(INT_1, &m_iTileCountY);
	GetMaterial()->UpdateData();

	// 구조화버퍼 업데이트
	UpdateData();

	// 렌더
	GetMesh()->render();
}

void CTileMap::UpdateData()
{
	// m_vecTile 내의 데이터 구조화버퍼로 전달
	m_Buffer->SetData(m_vecTile.data(), sizeof(tTile) * (UINT)m_vecTile.size());
	m_Buffer->UpdateData(20, PIPELINE_STAGE::PS_PIXEL);

	// m_TileMap 내의 데이터 구조화버퍼로 전달
	m_TileMapBuffer->SetData(m_TileMap.data(), sizeof(tTileMapIdx) * (UINT)m_TileMap.size());
	m_TileMapBuffer->UpdateData(22, PIPELINE_STAGE::PS_PIXEL);


}

void CTileMap::SetTileCount(UINT _iXCount, UINT _iYCount)
{
	m_iTileCountX = _iXCount;
	m_iTileCountY = _iYCount;

	// 벡터 크기 리사이즈
	m_vecTile.clear();
	m_vecTile.resize(m_iTileCountX * m_iTileCountY);

	// 구조화버퍼 사이즈보다 타일벡터 사이즈가 크다면 새로 만든다
	if (m_Buffer->GetElementCount() < m_vecTile.size())
	{
		m_Buffer->Create(sizeof(tTile), (UINT)m_vecTile.size(),SB_TYPE::READ_ONLY, true);
	}

	// 타일 세팅 테스트
	for (size_t i = 0; i < m_iTileCountY; ++i)
	{
		for (size_t j = 0; j < m_iTileCountX; ++j)
		{
			m_vecTile[i * m_iTileCountX + j].vLeftTop.x = m_vSliceSize.x * j;
			m_vecTile[i * m_iTileCountX + j].vLeftTop.y = m_vSliceSize.y * i;
			m_vecTile[i * m_iTileCountX + j].vSlice = m_vSliceSize;
		}
	}
	// 벡터 크기 리사이즈
	m_TileMap.clear();
	m_TileMap.resize(m_iTileCountX * m_iTileCountY);
	// 구조화버퍼 사이즈보다 타일벡터 사이즈가 크다면 새로 만든다
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
	// 부모 RenderComponent Save(Mesh, Mtrl)
	CRenderComponent::SaveToLevelFile(_File);

	fwrite(&m_iTileCountX, sizeof(UINT), 1, _File);
	fwrite(&m_iTileCountY, sizeof(UINT), 1, _File);
	fwrite(&m_vSliceSize, sizeof(Vec2), 1, _File);

	// 타일 데이터 저장
	fwrite(m_vecTile.data(), sizeof(tTile), m_vecTile.size(), _File);
}

void CTileMap::LoadFromLevelFile(FILE* _File)
{
	// 부모 RenderComponent Load (Mesh, Mtrl)
	CRenderComponent::LoadFromLevelFile(_File);

	fread(&m_iTileCountX, sizeof(UINT), 1, _File);
	fread(&m_iTileCountY, sizeof(UINT), 1, _File);
	fread(&m_vSliceSize, sizeof(Vec2), 1, _File);

	// 타일 가로세로 개수를 지정해준다 -> 버퍼가 resize 되며 생성됨.
	SetTileCount(m_iTileCountX, m_iTileCountY);

	// 타일 데이터 로드
	fread(m_vecTile.data(), sizeof(tTile), m_iTileCountX * m_iTileCountY, _File);
}

