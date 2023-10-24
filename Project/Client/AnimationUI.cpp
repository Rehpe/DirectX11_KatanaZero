#include "pch.h"
#include "AnimationUI.h"

#include <Engine\CKeyManager.h>
#include <Engine\CResourceManager.h>
#include <Engine\CAnim2D.h>
#include <Engine\CAnimator2D.h>

AnimationUI::AnimationUI()
	: UI("##AnimationTool")
	, m_iCol(1)
	, m_iRow(1)
	, m_bPause(false)
	, m_bRepeat(true)
	, m_pTargetObj(nullptr)
{

}

AnimationUI::~AnimationUI()
{
}

void AnimationUI::finaltick()
{
	// 부모의 finaltick 먼저 호출
	UI::finaltick();

	// Esc 눌리면 비활성화
	if (KEY_TAP(KEY::ESC))
	{
		SetActive(false);

		// 모든 UI 포커스 해제
		ImGui::SetWindowFocus(nullptr);
	}
}

int AnimationUI::render_update()
{
	Ptr<CTexture> pAnimAtlas = CResourceManager::GetInst()->FindRes<CTexture>(L"Link");
	ImTextureID AtlasID = (void*)(pAnimAtlas->GetSRV().Get());

	//=======Animation Frame Image===============

	ImGui::BeginChild("##Animation Info", ImVec2(0, 20.f));
	char string[50] = {};
	CAnim2D* CurAnim = m_pTargetObj->Animator2D()->GetCurAnim();

	WstringtoString(CurAnim->GetName(), string, 50);
	ImGui::Text("Current Animation:  ");
	ImGui::SameLine();
	ImGui::Text(string);
	ImGui::SameLine();
	ImGui::Text("  Total Frame: %d", CurAnim->GetTotalFrm());
	ImGui::EndChild();

	ImGui::BeginChild("##Animation Frame Image", ImVec2(0, 100.f), true, ImGuiWindowFlags_HorizontalScrollbar);

	// LeftTop, Slice, BackSize, Offset, Duration

	Vec2 vResolution = CurAnim->GetAtlasResolution();

	int iCurFrm = CurAnim->GetCurFrameint();
	Vec2 vLeftTopUV = CurAnim->GetCurFrame().LeftTopUV;
	Vec2 vSliceUV = CurAnim->GetCurFrame().SliceUV;
	Vec2 vOffsetUV = CurAnim->GetCurFrame().OffsetUV;
	Vec2 vBackSizeUV = CurAnim->GetBackSizeUV();
	float fDuration = CurAnim->GetCurFrame().fDuration;
	int iFPS = CurAnim->GetFPS();

	Vec2 vSlice = vSliceUV * vResolution;
	Vec2 vLeftTop = vLeftTopUV * vResolution;
	Vec2 vOffset = vOffsetUV * vResolution;
	Vec2 vBackSize = CurAnim->GetBackSize();

	for (int i = 0; i < CurAnim->GetTotalFrm(); i++)
	{
		char button_label[32];
		sprintf(button_label, "Button %d", i);

		const tAnim2DFrm& Frm = CurAnim->GetFrame(i);
		//ImGui::Text("[%d]", i);
		//ImGui::SameLine();
		ImVec4 transparent = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_Button, transparent);

		if (ImGui::ImageButton(button_label, AtlasID, ImVec2(60, 60)
			, ImVec2(Frm.LeftTopUV.x, Frm.LeftTopUV.y)
			, ImVec2((Frm.LeftTopUV + Frm.SliceUV).x, (Frm.LeftTopUV + Frm.SliceUV).y)))
		{

			if (m_bPause)
			{
				CurAnim->SetCurFrame(i);
			}
			else
			{
				m_pTargetObj->Animator2D()->Pause();
				m_bPause = true;
				CurAnim->SetCurFrame(i);
			}

		}
		ImGui::PopStyleColor();
		if (i != CurAnim->GetTotalFrm() - 1)
			ImGui::SameLine();

	}

	ImGui::EndChild();

	// ========== Animation Preview ==============

	ImGui::BeginChild("AnimPreview", ImVec2(250, 330.f), true);

	if (ImGui::ImageButton(AtlasID, ImVec2(180, 180), ImVec2(vLeftTopUV.x, vLeftTopUV.y), ImVec2((vLeftTopUV + vSliceUV).x, (vLeftTopUV + vSliceUV).y)))
	{
		if (!m_bPause)
		{
			m_pTargetObj->Animator2D()->Pause();
			m_bPause = true;
		}
		else
		{
			m_pTargetObj->Animator2D()->Pause();
			m_bPause = false;
		}
	}


	ImGui::EndChild();

	// ===========================================

	ImGui::SameLine();

	// ========== Animation Frm Info ==============
	vResolution = CurAnim->GetAtlasResolution();

	iCurFrm = CurAnim->GetCurFrameint();
	vLeftTopUV = CurAnim->GetCurFrame().LeftTopUV;
	vSliceUV = CurAnim->GetCurFrame().SliceUV;
	vOffsetUV = CurAnim->GetCurFrame().OffsetUV;
	vBackSizeUV = CurAnim->GetBackSizeUV();
	fDuration = CurAnim->GetCurFrame().fDuration;
	iFPS = CurAnim->GetFPS();

	vSlice = vSliceUV * vResolution;
	vLeftTop = vLeftTopUV * vResolution;
	vOffset = vOffsetUV * vResolution;
	vBackSize = CurAnim->GetBackSize();

	ImGui::BeginChild("FrmInfo", ImVec2(400, 330), true);

	ImGui::Text("Current Frame: %d", CurAnim->GetCurFrameint());
	ImGui::SameLine();
	m_bRepeat = m_pTargetObj->Animator2D()->IsRepeat();
	ImGui::Checkbox("Repeat", &m_bRepeat);
	m_pTargetObj->Animator2D()->SetRepeat(m_bRepeat);


	ImGui::Text("LeftTop    :");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(200);
	ImGui::DragFloat2("##LeftTop", (float*)&vLeftTop);

	ImGui::Text("LeftTopUV  :");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(200);
	ImGui::DragFloat2("##LeftTopUV", (float*)&vLeftTopUV);

	ImGui::Text("Slice      :");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(200);
	ImGui::DragFloat2("##Slice", (float*)&vSlice, 1.f, 0.f);

	ImGui::Text("SliceUV    :");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(200);
	ImGui::DragFloat2("##SliceUV", (float*)&vSliceUV, 1.f, 0.f);

	ImGui::Text("Offset     :");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(200);
	ImGui::DragFloat2("##Offset", (float*)&vOffset);

	ImGui::Text("OffsetUV   :");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(200);
	ImGui::DragFloat2("##OffsetUV", (float*)&vOffsetUV);

	ImGui::Text("BackSize   :");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(200);
	ImGui::DragFloat2("##BackSize", (float*)&vBackSize, 1.f, 0.f);

	ImGui::Text("BackSizeUV :");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(200);
	ImGui::DragFloat2("##BackSizeUV", (float*)&vBackSizeUV, 1.f, 0.f);

	ImGui::Text("Duration   :");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(200);
	ImGui::DragFloat("##vBackSize", &fDuration, 1.f, 0.f);

	ImGui::Text("FPS        :");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(200);
	ImGui::DragInt("##vFPS", &iFPS);

	CurAnim->SetCurFrmLeftTopUV(vLeftTop / vResolution);
	CurAnim->SetCurFrmSliceUV(vSlice / vResolution);
	CurAnim->SetCurFrmOffset(vOffset / vResolution);
	CurAnim->SetCurFrmDuration(1.f / iFPS);
	CurAnim->SetBackSize(vBackSize);
	CurAnim->SetFPS(iFPS);

	if (ImGui::Button("PopBack"))
	{
		if (CurAnim->GetTotalFrm() > 1)
			CurAnim->FrmPopBack();
	}

	ImGui::SameLine();

	if (ImGui::Button("Erase Frame"))
	{
		if (m_bPause)
		{
			if (!(CurAnim->GetTotalFrm() <= 1))
				CurAnim->DeleteFrm(iCurFrm);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Save"))
	{
		CurAnim->Save();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load"))
	{
		CurAnim->Load();
	}
	ImGui::SameLine();
	if (ImGui::Button("Save in Map"))
	{
		m_pTargetObj->Animator2D()->InsertAnim(CurAnim);
	}
	ImGui::SameLine();

	if (ImGui::Button("Atlas Setting"))
	{
		ImGui::OpenPopup("Atlas Setting");
	}
	if (ImGui::BeginPopup("Atlas Setting"))
	{

		ImGui::Text("Atlas Resolution: %d", int(vResolution.x));
		ImGui::SameLine();
		ImGui::Text("X %d", int(vResolution.y));

		ImGui::Text("LeftTop :");
		ImGui::SameLine();
		ImGui::Text("%f X %f", m_vStartPos.x, m_vStartPos.y);

		if (m_iCol < 1)
			m_iCol = 1;
		if (m_iRow < 1)
			m_iRow = 1;

		ImGui::Text("X   :");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::InputInt("##AtlasiCol", &m_iCol);
		ImGui::SameLine();
		ImGui::Text("Y   :");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::InputInt("##AtlasiRow", &m_iRow);
		ImGui::SameLine();
		static float Ratio = 0.5f;
		if (Ratio <= 0.f)
			Ratio = 0.1f;
		else if (Ratio >= 1.f)
			Ratio = 1.f;
		ImGui::Text("   Resolution Ratio   :");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		ImGui::InputFloat("##Ratio", &Ratio, (float)0.1, (float)0.1, " % .1f");

		Vec2 CurResolution = Vec2(vResolution.x * Ratio, vResolution.y * Ratio);

		if (m_iCol == 1 && m_iRow == 1)
		{
			ImGui::Image(AtlasID, ImVec2(CurResolution.x, CurResolution.y));

			if (DragRect(1))
			{
				ImVec2 LeftTop = m_vStartPos;
				ImVec2 Size = ImVec2(m_vEndPos.x - m_vStartPos.x, m_vEndPos.y - m_vStartPos.y);

				if (!(Size.x == 0 || Size.y == 0))
				{
					tAnim2DFrm frm = {};

					frm.LeftTopUV = Vec2(m_vStartPos.x / CurResolution.x, m_vStartPos.y / CurResolution.y);
					frm.SliceUV = Vec2(Size.x / CurResolution.x, Size.y / CurResolution.y);
					frm.OffsetUV = Vec2(0, 0);

					if (m_bPause)
					{
						CurAnim->AddFrame(frm);
					}
				}
			}
		}
		else
		{
			ImVec2 original_item_spacing = ImGui::GetStyle().ItemSpacing;
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

			Vec2 Slice = Vec2(CurResolution / Vec2(m_iCol, m_iRow));
			Vec2 SliceUV = Slice / CurResolution;

			for (int i = 0; i < m_iRow; i++)
			{
				for (int j = 0; j < m_iCol; j++)
				{
					ImVec2 LeftTop = ImVec2(j * Slice.x, i * Slice.y);

					char button_label[32];
					sprintf(button_label, "Button %d", i * m_iCol + j);

					ImVec2 LeftTopUV = ImVec2(LeftTop.x / CurResolution.x, LeftTop.y / CurResolution.y);

					if (ImGui::ImageButton(button_label, AtlasID
						, ImVec2(Slice.x, Slice.y)
						, ImVec2(LeftTopUV.x, LeftTopUV.y)
						, ImVec2((LeftTopUV.x + SliceUV.x), (LeftTopUV.y + SliceUV.y))))
					{
						tAnim2DFrm frm = {};

						frm.LeftTopUV = Vec2(LeftTopUV.x, LeftTopUV.y);
						frm.SliceUV = Vec2(SliceUV.x, SliceUV.y);
						frm.OffsetUV = Vec2(0, 0);

						CurAnim->AddFrame(frm);
					}

					if (j != m_iCol - 1)
						ImGui::SameLine();
				}
			}
			ImGui::PopStyleVar();
			ImGui::GetStyle().ItemSpacing = original_item_spacing;
		}

		ImGui::EndPopup();
	}
	ImGui::EndChild();

	return TRUE;
}


void AnimationUI::Reset(const string& _strName, ImVec2 _vSize)
{
	// List 이름, 사이즈, 활성화 여부 설정
	SetName(_strName);
	SetSize(_vSize.x, _vSize.y);
	SetActive(true);

	// 위치 조정
	HWND hWnd = ImGuiManager::GetInst()->GetMainHwnd();

	RECT rt = {};
	//GetClientRect(hWnd, &rt);         // Client : 윈도우만을 기준으로 좌상단을 0, 0으로 놓고 해상도만큼 
	GetWindowRect(hWnd, &rt);     // Window: 화면 전체를 기준으로 윈도우의 LT와 좌우 패딩을 뺀 순수 창 크기

	// 화면 중앙 좌표를 구한다.
	float CenterX = (float)((rt.left + rt.right) / 2);
	float CenterY = (float)((rt.bottom + rt.top) / 2);

	// 찾은 중앙좌표가 Popup창의 중앙이 될 수 있게 가로,세로 길이의 절반을 빼준다.
	SetPopupPos(ImVec2(CenterX - _vSize.x / 2.f, CenterY - _vSize.y / 2.f));
}

bool AnimationUI::DragRect(int _MouseBtn)
{
	ImVec2 AtlasStartPos = ImGui::GetItemRectMin();

	if (ImGui::IsMouseClicked(_MouseBtn))
		m_vStartPos = ImVec2(ImGui::GetMousePos().x - AtlasStartPos.x, ImGui::GetMousePos().y - AtlasStartPos.y);

	if (ImGui::IsMouseDown(_MouseBtn))
	{
		m_vEndPos = ImVec2(ImGui::GetMousePos().x - AtlasStartPos.x, ImGui::GetMousePos().y - AtlasStartPos.y);

		ImVec2 RectStart = ImVec2(m_vStartPos.x + AtlasStartPos.x, m_vStartPos.y + AtlasStartPos.y);
		ImVec2 RectEnd = ImVec2(m_vEndPos.x + AtlasStartPos.x, m_vEndPos.y + AtlasStartPos.y);

		// Draw the rectangle on top of the image
		ImDrawList* draw_list = ImGui::GetForegroundDrawList();
		draw_list->AddRect(RectStart, RectEnd, IM_COL32(255, 0, 0, 255));
	}

	return ImGui::IsMouseReleased(_MouseBtn);
}

