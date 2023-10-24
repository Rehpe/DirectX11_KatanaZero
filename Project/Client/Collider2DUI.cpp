#include "pch.h"
#include "Collider2DUI.h"

#include <Engine\CGameObject.h>
#include <Engine\CCollider2D.h>

#include <Engine\CKeyManager.h>

Collider2DUI::Collider2DUI()
    : ComponentUI("##Collider2D", COMPONENT_TYPE::COLLIDER2D)
{
    SetName("Collider2D");
}

Collider2DUI::~Collider2DUI()
{
}

int Collider2DUI::render_update()
{
    if (FALSE == ComponentUI::render_update())
        return FALSE;

    // 해당 UI에서 실제로 출력할 것
	bool IsAbsolute = GetTarget()->Collider2D()->IsAbsolute();
	Vec3 vOffsetPos = GetTarget()->Collider2D()->GetOffsetPos();
    Vec3 vOffsetScale = GetTarget()->Collider2D()->GetOffsetScale();
	Vec3 vStartPos = GetTarget()->Collider2D()->GetStartPos();
	Vec3 vEndPos = GetTarget()->Collider2D()->GetEndPos();

	COLLIDER2D_TYPE Shape = GetTarget()->Collider2D()->GetCollider2DType();
	int ColliderShape = (int)Shape;

	ImGui::Text("Collider Type");
	ImGui::SameLine();
	const char* ColliderType[] = { "Rect", "Circle", "Line" };
	if (ImGui::Combo("##ColliderType", &ColliderShape, ColliderType, IM_ARRAYSIZE(ColliderType)))
	{
		GetTarget()->Collider2D()->SetCollider2DType((COLLIDER2D_TYPE)ColliderShape);
	}

	if (ImGui::Checkbox("Absolute", &IsAbsolute))
	{
		GetTarget()->Collider2D()->SetAbsolute(IsAbsolute);
	}

	if (Shape == COLLIDER2D_TYPE::RECT)
	{
		ImGui::Text("Offset   ");
		ImGui::SameLine();
		ImGui::DragFloat2("##Relative Position", vOffsetPos);
		ImGui::Text("Scale	");
		ImGui::SameLine();
		ImGui::DragFloat2("##Relative Scale", vOffsetScale);
		static bool SameRatio = false;
		ImGui::SameLine();
		ImGui::Checkbox("Lock", &SameRatio);
		if (SameRatio)
		{
			vOffsetScale.y = vOffsetScale.x;
		}
		GetTarget()->Collider2D()->SetOffsetPos(Vec2(vOffsetPos.x, vOffsetPos.y));
		GetTarget()->Collider2D()->SetOffsetScale(Vec2(vOffsetScale.x, vOffsetScale.y));
	}

	else if (Shape == COLLIDER2D_TYPE::LINE)
	{
		static bool StartPosPicking = false;
		static bool EndPosPicking = false;

		ImGui::Text("StartPos   ");
		ImGui::SameLine();
		if (ImGui::DragFloat2("##StartPos", vStartPos))
		{
			GetTarget()->Collider2D()->SetStartPos(vStartPos);
		}
		ImGui::SameLine();
		ImGui::Checkbox("##StartPosPicking", &StartPosPicking);
		ImGui::Text("EndPos     ");
		ImGui::SameLine();
		if (ImGui::DragFloat2("##EndPos", vEndPos))
		{
			GetTarget()->Collider2D()->SetEndPos(vEndPos);
		}
		ImGui::SameLine();
		ImGui::Checkbox("##EndPosPicking", &EndPosPicking);

		if (StartPosPicking)
		{
			EndPosPicking = false;
			if (KEY_TAP(KEY::LBTN))
			{
				Vec2 MousePos = CKeyManager::GetInst()->GetProjMousePos();
				GetTarget()->Collider2D()->SetStartPos(Vec3(MousePos.x, MousePos.y,0));
				StartPosPicking = false;
			}
		}
		if (EndPosPicking)
		{
			StartPosPicking = false;
			if (KEY_TAP(KEY::LBTN))
			{
				Vec2 MousePos = CKeyManager::GetInst()->GetProjMousePos();
				GetTarget()->Collider2D()->SetEndPos(Vec3(MousePos.x, MousePos.y, 0));
				EndPosPicking = false;
			}
		}
	}
	else
	{

	}

    return TRUE;
}
