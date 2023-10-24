#include "pch.h"
#include "CPlayerIdleState.h"

#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>

#include <Engine\CKeyManager.h>
#include <Engine\CRigidBody2D.h>

#include "CScriptManager.h"
#include "CPlayerScript.h"

CPlayerIdleState::CPlayerIdleState()
{
	SetName(L"PlayerIdle");
}

CPlayerIdleState::~CPlayerIdleState()
{
}

void CPlayerIdleState::finaltick()
{
	CGameObject* pPlayer = GetOwner();
	CPlayerScript* pPlayerScript = pPlayer->GetScript<CPlayerScript>();

	bool IsGround = pPlayer->RigidBody2D()->IsGround();
	bool IsOnSlope = pPlayer->RigidBody2D()->IsOnSlope();
	// �̵�

	// Key Pressed �ϴ°� Player Script���� �ϰ�,
	// ���⼭�� Velocity ������ �� üũ�ϴ°� �´µ�
	
	// Jump
	if ((!IsGround && !IsOnSlope) && pPlayer->RigidBody2D()->GetVelocity().y > 0)
	{
		pPlayerScript->SetJump(true);
		GetOwnerFSM()->ChangeState(L"PlayerJump");
		return;
	}
	
	// IdleToRun
	if ((IsGround || IsOnSlope) && abs(pPlayer->RigidBody2D()->GetVelocity().x) > 0)
	{
		GetOwnerFSM()->ChangeState(L"PlayerIdleToRun");
		return;
	}

	// Crouch
	if ((IsGround || IsOnSlope) && KEY_PRESSED(KEY::S))//pPlayerScript->GetInputY() < 0)
	{
		GetOwnerFSM()->ChangeState(L"PlayerPreCrouch");
		return;
	}
	


}

void CPlayerIdleState::Enter()
{
	// Idle Anim ���
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"PlayerIdle")->Reset();
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"PlayerIdle", true);
}

void CPlayerIdleState::Exit()
{
	// Idle Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"PlayerIdle")->Reset();
}
