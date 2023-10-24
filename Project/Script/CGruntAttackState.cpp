#include "pch.h"
#include "CGruntAttackState.h"
#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>
#include <Engine\CTransform.h>
#include <Engine\CRigidBody2D.h>
#include "CEnemyGruntScript.h"
#include "CEnemyScript.h"
#include "CGruntAttackSpriteScript.h"

CGruntAttackState::CGruntAttackState()
{
	SetName(L"Attack");
}

CGruntAttackState::~CGruntAttackState()
{
}

void CGruntAttackState::finaltick()
{
	GetOwner()->RigidBody2D()->SetVelocity(Vec3(0.f, 0.f, 0.f));

	if (GetOwnerFSM()->GetOwner()->Animator2D()->GetCurAnim()->IsFinish())
	{
		GetOwnerFSM()->ChangeState(L"Idle");
	}

}

void CGruntAttackState::Enter()
{
	CGameObject* pEnemy = GetOwner();
	CEnemyScript* EnemyScript = dynamic_cast<CEnemyScript*>(GetOwner()->GetScripts()[0]);

	// GruntAttack Anim 재생
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"GruntAttack", false);

	EnemyScript->SetAttackable(false);

	// Attack Sprite 생성
	CGameObject* AttackSprite = GetOwner()->GetChildObjByName(L"GruntAttackSprite");
	CGruntAttackSpriteScript* AttackSpriteScript = dynamic_cast<CGruntAttackSpriteScript*>(AttackSprite->GetScripts()[0]);

	AttackSpriteScript->SetActive(true);
}

void CGruntAttackState::Exit()
{
	// GruntAttack Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"GruntAttack")->Reset();
}
