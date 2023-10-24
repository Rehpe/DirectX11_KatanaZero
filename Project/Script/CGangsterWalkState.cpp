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

	// �ӵ� ����
	pEnemy->RigidBody2D()->SetVelocityLimitX(80.f);

	// LookDir �������� ����
	int LookDir = EnemyScript->GetLookDir();
	float fWalkSpeed = EnemyScript->GetWalkSpeed();
	pEnemy->RigidBody2D()->AddVelocity(Vec3((LookDir * fWalkSpeed), 0.f, 0.f));

	if (EnemyScript->IsDetectingDoor())
		return;

	// �÷��̾� ���� �� Aim State ��ȯ
	if (EnemyScript->IsDetectingPlayer() && GangsterScript->IsShootable())
	{
		EnemyScript->SetNoticed(true);
		pEnemy->FSM()->ChangeState(L"Aim");
	}
	else if (EnemyScript->IsCollidingWithPlayer() && EnemyScript->IsDetectingPlayer()
		&& EnemyScript->IsAttackable())
	{
		// �÷��̾�� �浹 ���� ��� && Attack������ ���
		GetOwner()->FSM()->ChangeState(L"Attack");
	}
	else if (EnemyScript->IsDetectingPlayer())
	{
		GetOwner()->FSM()->ChangeState(L"Run");
	}
}

void CGangsterWalkState::Enter()
{
	// GangsterWalk Anim ���
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"GangsterWalk", true);
}

void CGangsterWalkState::Exit()
{
	// GangsterWalk Anim Reset
	//GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"GangsterWalk")->Reset();
}
