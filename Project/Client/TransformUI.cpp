#include "pch.h"
#include "TransformUI.h"

#include <Engine\CGameObject.h>
#include <Engine\CTransform.h>

TransformUI::TransformUI()
	: ComponentUI("##Transform", COMPONENT_TYPE::TRANSFORM)
{
	SetName("Transform");
}

TransformUI::~TransformUI()
{
}

int TransformUI::render_update()
{
	// 부모의 render_update()가 실패하면 무조건 실패한다. 판단은 부모에서
	if (FALSE == ComponentUI::render_update())
		return FALSE;

	static bool Absolute = GetTarget()->Transform()->IsAbsolute();
	if (ImGui::Checkbox("Absolute", &Absolute))
	{
		GetTarget()->Transform()->SetAbsolute(Absolute);
	}

	// 해당 UI에서 실제로 출력할 것
	Vec3 vPos = GetTarget()->Transform()->GetRelativePos();
	Vec3 vScale = GetTarget()->Transform()->GetRelativeScale();
	Vec3 vRotation = GetTarget()->Transform()->GetRelativeRot();
	vRotation = (vRotation / XM_PI) * 180.f;		// 사용자 편의를 위해 라디안->Degree로 변환

	// InputFloat3 : 입력으로 숫자 입력 가능
	// DragFloat3  : 입력, 드래그로 숫자 입력/조절 가능

	ImGui::Text("Position");
	ImGui::SameLine();
	ImGui::DragFloat3("##Relative Position", vPos);

	ImGui::Text("Scale   ");
	ImGui::SameLine();
	ImGui::DragFloat3("##Relative Scale", vScale);
	static bool SameRatio = false;
	ImGui::SameLine();
	if (ImGui::Checkbox("Lock", &SameRatio))
	{
	}
	if (SameRatio)
	{
		vScale.y = vScale.x;
	}

	ImGui::Text("Rotation");
	ImGui::SameLine();
	ImGui::DragFloat3("##Relative Rotation", vRotation);

	GetTarget()->Transform()->SetRelativePos(vPos);
	GetTarget()->Transform()->SetRelativeScale(vScale);

	vRotation = (vRotation / 180.f) * XM_PI;
	GetTarget()->Transform()->SetRelativeRot(vRotation);

	return TRUE;
}
