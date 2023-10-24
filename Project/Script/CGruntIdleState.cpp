#include "pch.h"
#include "CGruntIdleState.h"
#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>
#include <Engine\CLevel.h>
#include "CEnemyGruntScript.h"
#include "CEnemyScript.h"
#include "CPlayerScript.h"

CGruntIdleState::CGruntIdleState()
{
	SetName(L"Idle");
}

CGruntIdleState::~CGruntIdleState()
{
}

void CGruntIdleState::finaltick()
{
	CGameObject* pEnemy = GetOwner();
	CEnemyScript* EnemyScript = dynamic_cast<CEnemyScript*>(GetOwner()->GetScripts()[0]);
	CLevel* pLevel = CLevelManager::GetInst()->GetCurLevel();
	CGameObject* pPlayer = pLevel->FindObjectByName(L"Player");
	CPlayerScript*	PlayerScript = pPlayer->GetScript<CPlayerScript>();

	if (PlayerScript->IsDead())
	{
		return;
	}

	if (EnemyScript->IsNoticed())
	{
		if (EnemyScript->IsCollidingWithPlayer() && EnemyScript->IsAttackable())
		{
			// 플레이어와 충돌 중일 경우 && Attack가능할 경우
			GetOwner()->FSM()->ChangeState(L"Attack");
			return;
		}
		if (!EnemyScript->IsCollidingWithPlayer() && !EnemyScript->IsDetectingDoor())
		{
			// 플레이어를 감지중이나 충돌 중은 아닐 경우
			GetOwner()->FSM()->ChangeState(L"Run");
			return;
		}
		if (!EnemyScript->IsCollidingWithPlayer() && EnemyScript->IsDetectingDoor())
		{
			// 플레이어를 감지중이나 충돌 중은 아닐 경우
			GetOwner()->FSM()->ChangeState(L"Idle");
			return;
		}
		

	}
	else
	{
		// Turn 에서 전환될 경우
		GetOwner()->FSM()->ChangeState(L"Walk");
	}

}

void CGruntIdleState::Enter()
{
	// GruntIdle Anim 재생
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"GruntIdle", true);
}

void CGruntIdleState::Exit()
{
	// GruntIdle Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"GruntIdle")->Reset();
}
