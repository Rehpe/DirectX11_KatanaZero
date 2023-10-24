#include "pch.h"
#include "RigidBody2DUI.h"

#include <Engine\CGameObject.h>
#include <Engine\CRigidBody2D.h>

RigidBody2DUI::RigidBody2DUI()
	: ComponentUI("##RigidBody2D", COMPONENT_TYPE::RIGIDBODY2D)
{
	SetName("RigidBody2D");
}

RigidBody2DUI::~RigidBody2DUI()
{
}

void RigidBody2DUI::init()
{
}

int RigidBody2DUI::render_update()
{
	if (FALSE == ComponentUI::render_update())
		return FALSE;

	// 해당 UI에서 실제로 출력할 것
	float fMass = GetTarget()->RigidBody2D()->GetMass();	// 질량
	Vec3 vVelocity = GetTarget()->RigidBody2D()->GetVelocity();	// 속력
	Vec3 vForce = GetTarget()->RigidBody2D()->GetForce();	// 가해진 힘
	bool IsGround = GetTarget()->RigidBody2D()->IsGround();


	ImGui::Text("Mass    :");
	ImGui::SameLine();
	ImGui::DragFloat("##Mass", &fMass);

	ImGui::Text("Velocity:");
	ImGui::SameLine();
	ImGui::DragFloat3("##Velocity", vVelocity);

	ImGui::Text("Force   :");
	ImGui::SameLine();
	ImGui::DragFloat3("##Force", vForce);

	ImGui::Text("IsGround:");
	ImGui::SameLine();
	if (IsGround)
		ImGui::Text("True");
	else
		ImGui::Text("False");


	return 0;
}
