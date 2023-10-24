#include "pch.h"
#include "CGangsterIdleState.h"

#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>
#include <Engine\CLevel.h>

#include "CEnemyScript.h"
#include "CPlayerScript.h"
#include "CEnemyGangsterScript.h"

CGangsterIdleState::CGangsterIdleState()
{
	SetName(L"Idle");
}

CGangsterIdleState::~CGangsterIdleState()
{
}

void CGangsterIdleState::finaltick()
{
	CGameObject* pEnemy = GetOwner();
	CEnemyScript* EnemyScript = dynamic_cast<CEnemyScript*>(GetOwner()->GetScripts()[0]);
	CEnemyGangsterScript* GangsterScript = dynamic_cast<CEnemyGangsterScript*>(GetOwner()->GetScripts()[0]);
	CLevel* pLevel = CLevelManager::GetInst()->GetCurLevel();
	CGameObject* pPlayer = pLevel->FindObjectByName(L"Player");
	CPlayerScript* PlayerScript = pPlayer->GetScript<CPlayerScript>();

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
		}
		
		else if (!EnemyScript->IsCollidingWithPlayer() && EnemyScript->IsDetectingPlayer()
			&& GangsterScript->IsShootable())
		{
			// 플레이어를 감지중이나 충돌 중은 아닐 경우 : 총 쏘기
			GetOwner()->FSM()->ChangeState(L"Aim");
		}

		else if (!EnemyScript->IsCollidingWithPlayer() && !EnemyScript->IsDetectingPlayer())
		{
			// 둘 다 아닐 경우 Run
			GetOwner()->FSM()->ChangeState(L"Run");
		}
	}
	else
	{
		// Turn 에서 전환될 경우
		GetOwner()->FSM()->ChangeState(L"Walk");
	}
}

void CGangsterIdleState::Enter()
{
	// GangsterIdle Anim 재생
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"GangsterIdle", true);
}

void CGangsterIdleState::Exit()
{
	// GangsterIdle Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"GangsterIdle")->Reset();
}
