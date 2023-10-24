#include "pch.h"
#include "CPlayerHurtGroundState.h"

#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>

#include <Engine\CKeyManager.h>
#include <Engine\CRigidBody2D.h>

#include "CScriptManager.h"
#include "CPlayerScript.h"

CPlayerHurtGroundState::CPlayerHurtGroundState()
{
	SetName(L"PlayerHurtGround");
}

CPlayerHurtGroundState::~CPlayerHurtGroundState()
{
}

void CPlayerHurtGroundState::finaltick()
{
	CGameObject* pPlayer = GetOwner();
	CPlayerScript* pPlayerScript = pPlayer->GetScript<CPlayerScript>();


}

void CPlayerHurtGroundState::Enter()
{
	// PlayerHurtGround Anim Àç»ý
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"PlayerHurtGround", false);
}

void CPlayerHurtGroundState::Exit()
{
	// PlayerHurtGround Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"PlayerHurtGround")->Reset();
}
