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

	// �ӵ� ����
	pEnemy->RigidBody2D()->SetVelocityLimitX(80.f);
	
	// LookDir �������� ����
	int LookDir = EnemyScript->GetLookDir();
	float fWalkSpeed = EnemyScript->GetWalkSpeed();
	pEnemy->RigidBody2D()->AddVelocity(Vec3((LookDir * fWalkSpeed), 0.f, 0.f));

	if (EnemyScript->IsDetectingDoor())
		return;

	// �÷��̾� ���� �� Run State ��ȯ
	if(EnemyScript->IsDetectingPlayer())
	{
		pEnemy->FSM()->ChangeState(L"Run");
	}

}

void CGruntWalkState::Enter()
{
	// GruntWalk Anim ���
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"GruntWalk", true);
}

void CGruntWalkState::Exit()
{
	// GruntWalk Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"GruntWalk")->Reset();
}
