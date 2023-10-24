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
	// �θ��� render_update()�� �����ϸ� ������ �����Ѵ�. �Ǵ��� �θ𿡼�
	if (FALSE == ComponentUI::render_update())
		return FALSE;

	static bool Absolute = GetTarget()->Transform()->IsAbsolute();
	if (ImGui::Checkbox("Absolute", &Absolute))
	{
		GetTarget()->Transform()->SetAbsolute(Absolute);
	}

	// �ش� UI���� ������ ����� ��
	Vec3 vPos = GetTarget()->Transform()->GetRelativePos();
	Vec3 vScale = GetTarget()->Transform()->GetRelativeScale();
	Vec3 vRotation = GetTarget()->Transform()->GetRelativeRot();
	vRotation = (vRotation / XM_PI) * 180.f;		// ����� ���Ǹ� ���� ����->Degree�� ��ȯ

	// InputFloat3 : �Է����� ���� �Է� ����
	// DragFloat3  : �Է�, �巡�׷� ���� �Է�/���� ����

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
