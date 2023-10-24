#include "pch.h"
#include "CGangsterTurnState.h"

#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>

#include "CEnemyScript.h"

CGangsterTurnState::CGangsterTurnState()
{
	SetName(L"Turn");
}

CGangsterTurnState::~CGangsterTurnState()
{
}

void CGangsterTurnState::finaltick()
{
	CGameObject* pEnemy = GetOwner();
	CEnemyScript* EnemyScript = dynamic_cast<CEnemyScript*>(GetOwner()->GetScripts()[0]);

	pEnemy->RigidBody2D()->SetVelocity(Vec3(0.f, 0.f, 0.f));

	if (GetOwner()->Animator2D()->GetCurAnim()->IsFinish())
	{
		GetOwnerFSM()->ChangeState(L"Idle");
	}
}

void CGangsterTurnState::Enter()
{
	// GangsterTurn Anim Àç»ý
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"GangsterTurn", false);
}

void CGangsterTurnState::Exit()
{
	// GangsterTurn Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"GangsterTurn")->Reset();
}
