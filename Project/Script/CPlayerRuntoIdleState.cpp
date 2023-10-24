#include "pch.h"
#include "CPlayerRuntoIdleState.h"

#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>

#include <Engine\CKeyManager.h>
#include <Engine\CRigidBody2D.h>

#include "CScriptManager.h"
#include "CPlayerScript.h"

CPlayerRuntoIdleState::CPlayerRuntoIdleState()
{
}

CPlayerRuntoIdleState::~CPlayerRuntoIdleState()
{
}

void CPlayerRuntoIdleState::finaltick()
{
	CGameObject* pPlayer = GetOwner();
	CPlayerScript* pPlayerScript = pPlayer->GetScript<CPlayerScript>();


	// Jump
	if (pPlayer->RigidBody2D()->GetVelocity().y > 0)//pPlayerScript->IsJump() &&)
	{
		pPlayerScript->SetJump(true);
		GetOwnerFSM()->ChangeState(L"PlayerJump");
		return;
	}

	// Fall

	// Roll

	// Idle
	if (abs(pPlayer->RigidBody2D()->GetVelocity().x) < 0.01f)		// x�� ���ӵ��� 0�̸�
	{
		if (GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"PlayerRunToIdle")->IsFinish())
		{
			// RunToIdle �ִϸ��̼��� ����Ǿ��ٸ� 
			GetOwnerFSM()->ChangeState(L"PlayerIdle");
		}
	}

	// IdleToRun
	if (abs(pPlayer->RigidBody2D()->GetVelocity().x) > 0.1f)	// x�� ���ӵ��� �ణ ������
	{
		if (GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"PlayerRunToIdle")->IsFinish())
		{
			// RunToIdle �ִϸ��̼��� ����Ǿ��ٸ� 
			GetOwnerFSM()->ChangeState(L"PlayerIdleToRun");
		}
	}

}

void CPlayerRuntoIdleState::Enter()
{
	// RunToIdle Anim ���
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"PlayerRunToIdle", false);
}

void CPlayerRuntoIdleState::Exit()
{
	// RunToIdle Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"PlayerRunToIdle")->Reset();
}
