#include "pch.h"
#include "CPlayerPostCrouchState.h"

#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>

#include <Engine\CKeyManager.h>
#include <Engine\CRigidBody2D.h>

#include "CScriptManager.h"
#include "CPlayerScript.h"

CPlayerPostCrouchState::CPlayerPostCrouchState()
{
	SetName(L"PlayerPostCrouch");
}

CPlayerPostCrouchState::~CPlayerPostCrouchState()
{
}

void CPlayerPostCrouchState::finaltick()
{
	CGameObject* pPlayer = GetOwner();
	CPlayerScript* pPlayerScript = pPlayer->GetScript<CPlayerScript>();

	bool IsGround = pPlayer->RigidBody2D()->IsGround();
	bool IsOnSlope = pPlayer->RigidBody2D()->IsOnSlope();

	if (GetOwnerFSM()->GetOwner()->Animator2D()->GetCurAnim()->IsFinish())
		GetOwnerFSM()->ChangeState(L"PlayerIdle");
}

void CPlayerPostCrouchState::Enter()
{
	// Idle Anim Àç»ý
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"PlayerPostCrouch", false);

}

void CPlayerPostCrouchState::Exit()
{
	// Idle Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"PlayerPostCrouch")->Reset();
}
