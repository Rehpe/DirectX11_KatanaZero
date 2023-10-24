#include "pch.h"
#include "CPlayerCrouchState.h"

#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>

#include <Engine\CKeyManager.h>
#include <Engine\CRigidBody2D.h>
#include <Engine\CKeyManager.h>

#include "CScriptManager.h"
#include "CPlayerScript.h"

CPlayerCrouchState::CPlayerCrouchState()
{
	SetName(L"PlayerCrouch");
}

CPlayerCrouchState::~CPlayerCrouchState()
{
}

void CPlayerCrouchState::finaltick()
{
	CGameObject* pPlayer = GetOwner();
	CPlayerScript* pPlayerScript = pPlayer->GetScript<CPlayerScript>();

	bool IsGround = pPlayer->RigidBody2D()->IsGround();
	bool IsOnSlope = pPlayer->RigidBody2D()->IsOnSlope();

	if ((KEY_PRESSED(KEY::S) && (KEY_PRESSED(KEY::A))))
	{
		pPlayerScript->SetLookDir(-1);
		pPlayerScript->SetRoll(true);
		GetOwnerFSM()->ChangeState(L"PlayerRoll");
		return;
	}
	else if ((KEY_PRESSED(KEY::S) && (KEY_PRESSED(KEY::D))))
	{
		pPlayerScript->SetLookDir(1);
		pPlayerScript->SetRoll(true);
		GetOwnerFSM()->ChangeState(L"PlayerRoll");
		return;
	}

	if (KEY_PRESSED(KEY::S))
	{
		return;
	}
	
	//if(KEY_RELEASED(KEY::S))
		GetOwnerFSM()->ChangeState(L"PlayerPostCrouch");
}

void CPlayerCrouchState::Enter()
{
	// Crouch Anim Àç»ý
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"PlayerCrouch", true);
}

void CPlayerCrouchState::Exit()
{
	// Crouch Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"PlayerCrouch")->Reset();
}
