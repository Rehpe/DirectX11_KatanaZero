#include "pch.h"
#include "CGangsterWalkState.h"

#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>

#include "CEnemyScript.h"
#include "CEnemyGangsterScript.h"

CGangsterWalkState::CGangsterWalkState()
{
	SetName(L"Walk");
}

CGangsterWalkState::~CGangsterWalkState()
{
}

void CGangsterWalkState::finaltick()
{
	CGameObject* pEnemy = GetOwner();
	CEnemyScript* EnemyScript = dynamic_cast<CEnemyScript*>(GetOwner()->GetScripts()[0]);
	CEnemyGangsterScript* GangsterScript = dynamic_cast<CEnemyGangsterScript*>(GetOwner()->GetScripts()[0]);

	// 속도 제한
	pEnemy->RigidBody2D()->SetVelocityLimitX(80.f);

	// LookDir 방향으로 전진
	int LookDir = EnemyScript->GetLookDir();
	float fWalkSpeed = EnemyScript->GetWalkSpeed();
	pEnemy->RigidBody2D()->AddVelocity(Vec3((LookDir * fWalkSpeed), 0.f, 0.f));

	if (EnemyScript->IsDetectingDoor())
		return;

	// 플레이어 감지 시 Aim State 전환
	if (EnemyScript->IsDetectingPlayer() && GangsterScript->IsShootable())
	{
		EnemyScript->SetNoticed(true);
		pEnemy->FSM()->ChangeState(L"Aim");
	}
	else if (EnemyScript->IsCollidingWithPlayer() && EnemyScript->IsDetectingPlayer()
		&& EnemyScript->IsAttackable())
	{
		// 플레이어와 충돌 중일 경우 && Attack가능할 경우
		GetOwner()->FSM()->ChangeState(L"Attack");
	}
	else if (EnemyScript->IsDetectingPlayer())
	{
		GetOwner()->FSM()->ChangeState(L"Run");
	}
}

void CGangsterWalkState::Enter()
{
	// GangsterWalk Anim 재생
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"GangsterWalk", true);
}

void CGangsterWalkState::Exit()
{
	// GangsterWalk Anim Reset
	//GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"GangsterWalk")->Reset();
}
