#include "pch.h"
#include "CGangsterAttackState.h"

#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>

#include "CEnemyScript.h"
#include "CGruntAttackSpriteScript.h"

CGangsterAttackState::CGangsterAttackState()
{
	SetName(L"Attack");
}

CGangsterAttackState::~CGangsterAttackState()
{
}

void CGangsterAttackState::finaltick()
{
	GetOwner()->RigidBody2D()->SetVelocity(Vec3(0.f, 0.f, 0.f));

	if (GetOwnerFSM()->GetOwner()->Animator2D()->GetCurAnim()->IsFinish())
	{
		GetOwnerFSM()->ChangeState(L"Idle");
	}
}

void CGangsterAttackState::Enter()
{
	// GangsterAttack Anim 재생
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"GangsterAttack", false);

	CGameObject* pEnemy = GetOwner();
	CEnemyScript* EnemyScript = dynamic_cast<CEnemyScript*>(GetOwner()->GetScripts()[0]);

	EnemyScript->SetAttackable(false);

	// Attack Sprite 생성
	CGameObject* AttackSprite = GetOwner()->GetChildObjByName(L"GruntAttackSprite");
	CGruntAttackSpriteScript* AttackSpriteScript = dynamic_cast<CGruntAttackSpriteScript*>(AttackSprite->GetScripts()[0]);

	AttackSpriteScript->SetActive(true);
}

void CGangsterAttackState::Exit()
{
	// GangsterAttack Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"GangsterAttack")->Reset();
}
