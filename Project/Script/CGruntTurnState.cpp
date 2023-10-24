#include "pch.h"
#include "CGruntTurnState.h"
#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>
#include "CEnemyGruntScript.h"
#include "CEnemyScript.h"

CGruntTurnState::CGruntTurnState()
{
	SetName(L"Turn");
}

CGruntTurnState::~CGruntTurnState()
{
}

void CGruntTurnState::finaltick()
{
	CGameObject* pEnemy = GetOwner();
	CEnemyScript* EnemyScript = dynamic_cast<CEnemyScript*>(GetOwner()->GetScripts()[0]);

	pEnemy->RigidBody2D()->SetVelocity(Vec3(0.f, 0.f, 0.f));

	if (GetOwner()->Animator2D()->GetCurAnim()->IsFinish())
	{
		GetOwnerFSM()->ChangeState(L"Idle");
	}
}

void CGruntTurnState::Enter()
{
	// GruntTurn Anim Àç»ý
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"GruntTurn", false);

}

void CGruntTurnState::Exit()
{
	// GruntTurn Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"GruntTurn")->Reset();
}
