#include "pch.h"
#include "CGruntWalkState.h"

#include "CEnemyScript.h"

#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>

CGruntWalkState::CGruntWalkState()
{
	SetName(L"Walk");
}

CGruntWalkState::~CGruntWalkState()
{
}

void CGruntWalkState::finaltick()
{
	CGameObject* pEnemy = GetOwner();
	CEnemyScript* EnemyScript = dynamic_cast<CEnemyScript*>(GetOwner()->GetScripts()[0]);

	// 속도 제한
	pEnemy->RigidBody2D()->SetVelocityLimitX(80.f);
	
	// LookDir 방향으로 전진
	int LookDir = EnemyScript->GetLookDir();
	float fWalkSpeed = EnemyScript->GetWalkSpeed();
	pEnemy->RigidBody2D()->AddVelocity(Vec3((LookDir * fWalkSpeed), 0.f, 0.f));

	if (EnemyScript->IsDetectingDoor())
		return;

	// 플레이어 감지 시 Run State 전환
	if(EnemyScript->IsDetectingPlayer())
	{
		pEnemy->FSM()->ChangeState(L"Run");
	}

}

void CGruntWalkState::Enter()
{
	// GruntWalk Anim 재생
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"GruntWalk", true);
}

void CGruntWalkState::Exit()
{
	// GruntWalk Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"GruntWalk")->Reset();
}
