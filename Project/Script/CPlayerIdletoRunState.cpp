#include "pch.h"
#include "CPlayerIdletoRunState.h"

#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>

#include <Engine\CKeyManager.h>
#include <Engine\CRigidBody2D.h>

#include "CScriptManager.h"
#include "CPlayerScript.h"
#include "CEffectManager.h"

CPlayerIdletoRunState::CPlayerIdletoRunState()
{
	SetName(L"PlayerIdleToRun");
}

CPlayerIdletoRunState::~CPlayerIdletoRunState()
{
}

void CPlayerIdletoRunState::finaltick()
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

	// Run
	if (abs(pPlayer->RigidBody2D()->GetVelocity().x) >= 0.1f)		// x�� ���ӵ��� �����̶� ���� ��
	{
		if (GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"PlayerIdleToRun")->IsFinish())
		{
			// IdleToRun �ִϸ��̼��� ����Ǿ��ٸ� 
			GetOwnerFSM()->ChangeState(L"PlayerRun");
		}
	}

	// RunToIdle
	if (abs(pPlayer->RigidBody2D()->GetVelocity().x) < 0)		// x�� ���ӵ��� 0�� ��
	{
		GetOwnerFSM()->ChangeState(L"PlayerRunToIdle");
	}


}

void CPlayerIdletoRunState::Enter()
{
	// IdleToRun Anim ���
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"PlayerIdleToRun")->Reset();
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"PlayerIdleToRun", false);

	Vec3 PlayerPos = GetOwner()->Transform()->GetRelativePos();
	Vec3 PlayerScale = GetOwner()->Collider2D()->GetOffsetScale();
	Vec3 Pos = PlayerPos - Vec3(PlayerScale / 2.f);
	// Player �� �ؿ� DustCloud Effect ����
	CEffectManager::CreateEffect(EffectType::DUSTCLOUD, Pos, true);
	
}

void CPlayerIdletoRunState::Exit()
{
	// IdleToRun Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"PlayerIdleToRun")->Reset();
}
