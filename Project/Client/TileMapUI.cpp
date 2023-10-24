#include "pch.h"
#include "TileMapUI.h"

#include <Engine\CGameObject.h>
#include <Engine\CTileMap.h>
#include <Engine\CTransform.h>
#include <Engine\function.h>

TileMapUI::TileMapUI()
	: ComponentUI("##TileMap", COMPONENT_TYPE::TILEMAP)
	, m_iTileCountX(1)
	, m_iTileCountY(1)
	, m_iSliceX(1)
	, m_iSliceY(1)
	, m_SelectedTileIdx(0)
	, m_fRatio(0.5f)
	, m_bNewTileSetPopup(true)
{
	SetName("TileMap");

}

TileMapUI::~TileMapUI()
{
	m_vecTile.clear();
	m_vecTileMap.clear();
}

void TileMapUI::init()
{
	// 타일 아틀라스
	m_TileAtlas = GetTarget()->TileMap()->GetTileAtlas();
	m_TileResolution = Vec2(m_TileAtlas->Width(), m_TileAtlas->Height());

	m_iTileCountX = GetTarget()->TileMap()->GetTileCountX();
	m_iTileCountY = GetTarget()->TileMap()->GetTileCountY();

	m_iSliceX = GetTarget()->TileMap()->GetSliceXCount();
	m_iSliceY = GetTarget()->TileMap()->GetSliceYCount();

	m_vecTile = GetTarget()->TileMap()->GetTileVector();

	m_vecTileMap = GetTarget()->TileMap()->GetTileMapIdx();
} 

int TileMapUI::render_update()
{
	if (FALSE == ComponentUI::render_update())
		return FALSE;


	// ====== 타일 갯수 =======
	ImGui::Text("X : %d", GetTarget()->TileMap()->GetTileCountX());
	ImGui::Text("Y : %d", GetTarget()->TileMap()->GetTileCountY());

	Vec2 CurResolution = m_TileResolution * m_fRatio;

	static bool Active = true;
	// ====== 타일맵 수정 =======
	if (ImGui::Button("Edit TileMap"))
	{
		ImGui::OpenPopup("TilemapEditor");
	}

	if (ImGui::BeginPopupModal("TilemapEditor"))
	{
		// ======== 1. 현재 엔진에서 사용중인 타일셋 정보 & 새로운 타일맵 생성 버튼
		ImGui::BeginChild("CurrentTileSetInfo", ImVec2(ImGui::GetWindowWidth() / 2, 120), true);

		// 1-1.  현재 엔진에서 사용중인 타일셋 정보
		ImGui::Text("Current TileSet Info");
		
		char szAtlasName[50] = {};
		WstringtoString(m_TileAtlas->GetKey(), szAtlasName, 50);
		ImGui::BulletText(szAtlasName);

		ImGui::BulletText("SliceX : %d", m_iSliceX);
		ImGui::SameLine();
		ImGui::Text("SliceX : %d", m_iSliceY);
		ImGui::BulletText("Total %d Tiles", m_iSliceX * m_iSliceY);
		// 1-2. 새로운 타일맵 생성 버튼
		CreateNewTileSetPopup();
		ImGui::EndChild();

		ImGui::SameLine();

		// ========= 2.  현재 선택된 타일 인덱스 보여주기
		ImGui::BeginChild("SelectedTile", ImVec2(ImGui::GetWindowWidth() / 2, 120), true);

		ImGui::Text("Selected Tile : %d", m_SelectedTileIdx);
		ImTextureID ID = m_TileAtlas->GetSRV().Get();
		if (m_SelectedTileIdx >= int(m_vecTile.size()))
		{
			m_SelectedTileIdx = 0;
		}
		if (m_SelectedTileIdx == -1)
		{
			Vec2 LeftTopPixel = Vec2(0, 0);
			ImVec2 LeftTopUV = ImVec2(0,0);

			Vec2 SlicePixel = m_vecTile[0].vSlice * m_TileResolution;
			Vec2 CurResolSlicePixel = m_vecTile[0].vSlice * CurResolution;

			ImVec2 SliceUV = ImVec2(SlicePixel.x / m_TileResolution.x, SlicePixel.y / m_TileResolution.y);
			ID = nullptr;

			ImGui::ImageButton(ID
				, ImVec2(CurResolSlicePixel.x, CurResolSlicePixel.y)
				, ImVec2(LeftTopUV.x, LeftTopUV.y)
				, ImVec2((LeftTopUV.x + SliceUV.x), (LeftTopUV.y + SliceUV.y)));
		}
		else
		{
			Vec2 LeftTopPixel = m_vecTile[m_SelectedTileIdx].vLeftTop * m_TileResolution;
			ImVec2 LeftTopUV = ImVec2(LeftTopPixel.x / m_TileResolution.x, LeftTopPixel.y / m_TileResolution.y);

			Vec2 SlicePixel = m_vecTile[m_SelectedTileIdx].vSlice * m_TileResolution;
			Vec2 CurResolSlicePixel = m_vecTile[m_SelectedTileIdx].vSlice * CurResolution;

			ImVec2 SliceUV = ImVec2(SlicePixel.x / m_TileResolution.x, SlicePixel.y / m_TileResolution.y);

			ImGui::ImageButton(m_TileAtlas->GetSRV().Get()
				, ImVec2(CurResolSlicePixel.x, CurResolSlicePixel.y)
				, ImVec2(LeftTopUV.x, LeftTopUV.y)
				, ImVec2((LeftTopUV.x + SliceUV.x), (LeftTopUV.y + SliceUV.y)));
		}
		ImGui::EndChild();

		// ======== 3. 현재 사용중인 타일셋 미리보기 / 선택
		//ImVec2 content_size = ImGui::GetItemRectSize();
		ImGui::BeginChild("Current TileSet", ImVec2(0, 300), true);

		ImVec2 original_item_spacing = ImGui::GetStyle().ItemSpacing;
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		for (int i = 0; i < m_iSliceY; i++)
		{
			for (int j = 0; j < m_iSliceX; j++)
			{
				char button_label[32];
				sprintf(button_label, "CurrentTileSet %d", i * m_iSliceX + j);

				Vec2 LeftTopPixel = m_vecTile[i * m_iSliceX + j].vLeftTop * m_TileResolution;
				ImVec2 LeftTopUV = ImVec2(LeftTopPixel.x / m_TileResolution.x, LeftTopPixel.y / m_TileResolution.y);

				Vec2 SlicePixel = m_vecTile[i * m_iSliceX + j].vSlice * m_TileResolution;
				Vec2 CurResolSlicePixel = m_vecTile[i * m_iSliceX + j].vSlice * CurResolution;

				ImVec2 SliceUV = ImVec2(SlicePixel.x / m_TileResolution.x, SlicePixel.y / m_TileResolution.y);

				if (ImGui::ImageButton(button_label, m_TileAtlas->GetSRV().Get()
					, ImVec2(CurResolSlicePixel.x, CurResolSlicePixel.y)
					, ImVec2(LeftTopUV.x, LeftTopUV.y)
					, ImVec2((LeftTopUV.x + SliceUV.x), (LeftTopUV.y + SliceUV.y))))
				{
					m_SelectedTileIdx = i * m_iSliceX + j;
				}

				if (j != m_iSliceX - 1)
					ImGui::SameLine();
			}
		}
		ImGui::PopStyleVar();
		ImGui::GetStyle().ItemSpacing = original_item_spacing;
		ImGui::EndChild();
		
		// ========== 4. 타일맵 행렬 수정 / 배율 변경 / 타일맵 편의기능
		ImGui::BeginChild("TileCount", ImVec2(ImGui::GetWindowWidth(), 100.f), true);

		// 4-1. 타일갯수 수정
		ImGui::Text("Tile Count");

		ImGui::Text("X :");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100.f);
		ImGui::InputInt("##TileX", &m_iTileCountX, 1);
		ImGui::SameLine();
		ImGui::Text("Y :");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100.f);
		ImGui::InputInt("##TileY", &m_iTileCountY, 1);

		if (m_iTileCountX <= 1)
			m_iTileCountX = 1;
		if (m_iTileCountY <= 1)
			m_iTileCountY = 1;

		ImGui::SameLine();

		// 4-2. 배율 변경
		ImGui::Text("Ratio :");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(50);
		ImGui::DragFloat("##Ratio", &m_fRatio, 0.1f, 0.1f, 1, "%.1f");

		// 4-3. Apply 버튼
		if (ImGui::Button("Apply"))
		{
			GetTarget()->TileMap()->SetTileAtlas(m_TileAtlas);
			GetTarget()->TileMap()->SetTileCount(m_iTileCountX, m_iTileCountY);
			GetTarget()->TileMap()->SetTileVector(m_vecTile);
			GetTarget()->TileMap()->SetTileMap(m_vecTileMap);
		}
		ImGui::SameLine();
		// 4-4. Fill All 버튼
		if (ImGui::Button("Fill All"))
		{
			for (int i = 0; i < m_vecTileMap.size(); i++)
			{
				m_vecTileMap[i].TileIdx = m_SelectedTileIdx;
			}
		}
		ImGui::SameLine();
		// 4-5. Erase All 버튼
		if (ImGui::Button("Erase All"))
		{
			for (int i = 0; i < m_vecTileMap.size(); i++)
			{
				m_vecTileMap[i].TileIdx = -1;
			}
		}
		ImGui::SameLine();
		// 4-6. Erase 버튼
		if (ImGui::Button("Erase"))
		{
			m_SelectedTileIdx = -1;
		}


		ImGui::EndChild();


		// ========= 5. TileMap Edit 미리보기
		ImGui::BeginChild("TileMap Edit", ImVec2(0, 500), true);

		Vec2 TileMapSize = Vec2(GetTarget()->Transform()->GetRelativeScale().x, GetTarget()->Transform()->GetRelativeScale().y) * m_fRatio;
		Vec2 TileMapSlice = TileMapSize / Vec2(m_iTileCountX, m_iTileCountY);

		if (m_vecTileMap.size() < m_iTileCountX * m_iTileCountY)
		{
			m_vecTileMap.resize(m_iTileCountX* m_iTileCountY);
		}
		
		// + 드래그로 선택기능
		static bool MouseClicked = false;
		
		if (ImGui::IsMouseClicked(0))
			MouseClicked = true;

		if (!ImGui::IsMouseDown(0))
			MouseClicked = false;

		for (int i = 0; i < m_iTileCountY; i++)
		{
			for (int j = 0; j < m_iTileCountX; j++)
			{
				//ImGui::PushID(i * m_iTileCountX + j);
				ImVec2 button_pos = ImGui::GetCursorScreenPos();
				ImVec2 mouse_pos = ImGui::GetIO().MousePos;

				if (ImGui::IsMouseHoveringRect(button_pos, ImVec2(button_pos.x + TileMapSlice.x, button_pos.y + TileMapSlice.y)) && MouseClicked)
				{
					m_vecTileMap[i * m_iTileCountX + j].TileIdx = m_SelectedTileIdx;
				}

				char button_label[32];
				sprintf(button_label, "CurrentTileMap %d", i * m_iTileCountX + j);

				Vec2 LeftTopUV;
				Vec2 SliceUV;
				ImTextureID ID = m_TileAtlas->GetSRV().Get();
				int TileIdx = m_vecTileMap[i * m_iTileCountX + j].TileIdx;

				if (TileIdx == -1)
				{
					LeftTopUV = m_vecTile[0].vLeftTop;
					SliceUV = m_vecTile[0].vSlice;
					ID = nullptr;
				}
				else if (int(m_vecTile.size()) <= TileIdx)
				{
					LeftTopUV = m_vecTile[0].vLeftTop;
					SliceUV = m_vecTile[0].vSlice;
				}
				
				else
				{
					LeftTopUV = m_vecTile[TileIdx].vLeftTop;
					SliceUV = m_vecTile[TileIdx].vSlice;
				}

				if (ImGui::ImageButton(button_label, ID
					, ImVec2(TileMapSlice.x, TileMapSlice.y)
					, ImVec2(LeftTopUV.x, LeftTopUV.y)
					, ImVec2((LeftTopUV.x + SliceUV.x), (LeftTopUV.y + SliceUV.y))))
				{
					m_vecTileMap[i * m_iTileCountX + j].TileIdx = m_SelectedTileIdx;
				}
				if (j != m_iTileCountX - 1)
					ImGui::SameLine();
			}
		}
		ImGui::EndChild();

		ImGui::EndPopup();
	}
	return TRUE;
}

void TileMapUI::CreateNewTileSetPopup()
{
	Vec2 CurResolution = m_TileResolution * m_fRatio;

	if (ImGui::Button("Create New TileSet"))
	{
		ImGui::OpenPopup("TileSet Editor");
	}
	if (ImGui::BeginPopupModal("TileSet Editor", &m_bNewTileSetPopup))
	{
		ImGui::BeginChild("Atlas Info", ImVec2(0, 150));

		// 아틀라스 텍스처 이름, 해상도
		char szAtlasName[50] = {};
		WstringtoString(m_TileAtlas->GetKey(), szAtlasName, 50);

		ImGui::BulletText(szAtlasName);
		ImGui::SameLine();
		ImGui::Text("Resolution: %.0f X %.0f", m_TileAtlas->Width(), m_TileAtlas->Height());

		// 타일 슬라이스 수정
		ImGui::Text("Tile Slice");
		ImGui::SameLine();

		static int NewSliceX = 1;
		static int NewSliceY = 1;

		if (NewSliceX <= 1)
			NewSliceX = 1;
		if (NewSliceY <= 1)
			NewSliceY = 1;

		ImGui::Text("X :");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::InputInt("##NewSliceX", &NewSliceX, 1, 1, 1);
		ImGui::SameLine();
		ImGui::Text("Y :");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::InputInt("##NewSliceY", &NewSliceY, 1, 1, 1);
		ImGui::SameLine();
		ImGui::Text("Total Tiles : %d", NewSliceX * NewSliceY);

		if (ImGui::Button("Create TileSet"))
		{

			// 기존 받아왔던 타일셋 clear 후 resize
			m_vecTile.clear();
			m_vecTile.resize(NewSliceX * NewSliceY);

			Vec2 SlicePixel = Vec2(CurResolution / Vec2(NewSliceX, NewSliceY));
			Vec2 SliceUV = Vec2(SlicePixel.x / CurResolution.x, SlicePixel.y / CurResolution.y);

			// 새로 만든 타일셋 저장
			for (int i = 0; i < NewSliceY; i++)
			{
				for (int j = 0; j < NewSliceX; j++)
				{
					ImVec2 LeftTopPixel = ImVec2(j * SlicePixel.x, i * SlicePixel.y);
					Vec2 LeftTopUV = Vec2(LeftTopPixel.x / CurResolution.x, LeftTopPixel.y / CurResolution.y);

					m_vecTile[i * NewSliceX + j].vLeftTop = LeftTopUV;
					m_vecTile[i * NewSliceX + j].vSlice = SliceUV;
				}
			}

			m_iSliceX = NewSliceX;
			m_iSliceY = NewSliceY;
		}

		ImGui::EndChild();


		ImGui::BeginChild("CutAtlas");

		Vec2 SlicePixel = Vec2(CurResolution / Vec2(NewSliceX, NewSliceY));
		ImVec2 SliceUV = ImVec2(SlicePixel.x / CurResolution.x, SlicePixel.y / CurResolution.y);

		ImVec2 original_item_spacing = ImGui::GetStyle().ItemSpacing;
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		// Tile Slice & Create
		for (int i = 0; i < NewSliceY; i++)
		{
			for (int j = 0; j < NewSliceX; j++)
			{
				ImVec2 LeftTopPixel = ImVec2(j * SlicePixel.x, i * SlicePixel.y);

				char button_label[32];
				sprintf(button_label, "Button %d", i * NewSliceX + j);

				ImVec2 LeftTopUV = ImVec2(LeftTopPixel.x / CurResolution.x, LeftTopPixel.y / CurResolution.y);

				if (ImGui::ImageButton(button_label, m_TileAtlas->GetSRV().Get()
					, ImVec2(SlicePixel.x, SlicePixel.y)
					, ImVec2(LeftTopUV.x, LeftTopUV.y)
					, ImVec2((LeftTopUV.x + SliceUV.x), (LeftTopUV.y + SliceUV.y))))
				{
				}

				
				if (j != NewSliceX - 1)
					ImGui::SameLine();
			}
		}

		ImGui::PopStyleVar();
		ImGui::GetStyle().ItemSpacing = original_item_spacing;


		ImGui::EndChild();


		ImGui::EndPopup();
	}
}
