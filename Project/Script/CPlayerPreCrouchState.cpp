#include "pch.h"
#include "CPlayerPreCrouchState.h"

#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>

#include <Engine\CKeyManager.h>
#include <Engine\CRigidBody2D.h>

#include "CScriptManager.h"
#include "CPlayerScript.h"

CPlayerPreCrouchState::CPlayerPreCrouchState()
{
	SetName(L"PlayerPreCrouch");
}

CPlayerPreCrouchState::~CPlayerPreCrouchState()
{
}

void CPlayerPreCrouchState::finaltick()
{
	CGameObject* pPlayer = GetOwner();
	CPlayerScript* pPlayerScript = pPlayer->GetScript<CPlayerScript>();

	bool IsGround = pPlayer->RigidBody2D()->IsGround();
	bool IsOnSlope = pPlayer->RigidBody2D()->IsOnSlope();

	if(GetOwnerFSM()->GetOwner()->Animator2D()->GetCurAnim()->IsFinish())
		GetOwnerFSM()->ChangeState(L"PlayerCrouch");
}

void CPlayerPreCrouchState::Enter()
{
	// Idle Anim Àç»ý
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"PlayerPreCrouch", false);

}

void CPlayerPreCrouchState::Exit()
{
	// Idle Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"PlayerPreCrouch")->Reset();
}
