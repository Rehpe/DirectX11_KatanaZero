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
			// �÷��̾�� �浹 ���� ��� && Attack������ ���
			GetOwner()->FSM()->ChangeState(L"Attack");
		}
		
		else if (!EnemyScript->IsCollidingWithPlayer() && EnemyScript->IsDetectingPlayer()
			&& GangsterScript->IsShootable())
		{
			// �÷��̾ �������̳� �浹 ���� �ƴ� ��� : �� ���
			GetOwner()->FSM()->ChangeState(L"Aim");
		}

		else if (!EnemyScript->IsCollidingWithPlayer() && !EnemyScript->IsDetectingPlayer())
		{
			// �� �� �ƴ� ��� Run
			GetOwner()->FSM()->ChangeState(L"Run");
		}
	}
	else
	{
		// Turn ���� ��ȯ�� ���
		GetOwner()->FSM()->ChangeState(L"Walk");
	}
}

void CGangsterIdleState::Enter()
{
	// GangsterIdle Anim ���
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"GangsterIdle", true);
}

void CGangsterIdleState::Exit()
{
	// GangsterIdle Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"GangsterIdle")->Reset();
}
