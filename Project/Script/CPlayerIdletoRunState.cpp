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
	if (abs(pPlayer->RigidBody2D()->GetVelocity().x) >= 0.1f)		// x축 가속도가 조금이라도 있을 때
	{
		if (GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"PlayerIdleToRun")->IsFinish())
		{
			// IdleToRun 애니메이션이 종료되었다면 
			GetOwnerFSM()->ChangeState(L"PlayerRun");
		}
	}

	// RunToIdle
	if (abs(pPlayer->RigidBody2D()->GetVelocity().x) < 0)		// x축 가속도가 0일 때
	{
		GetOwnerFSM()->ChangeState(L"PlayerRunToIdle");
	}


}

void CPlayerIdletoRunState::Enter()
{
	// IdleToRun Anim 재생
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"PlayerIdleToRun")->Reset();
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"PlayerIdleToRun", false);

	Vec3 PlayerPos = GetOwner()->Transform()->GetRelativePos();
	Vec3 PlayerScale = GetOwner()->Collider2D()->GetOffsetScale();
	Vec3 Pos = PlayerPos - Vec3(PlayerScale / 2.f);
	// Player 발 밑에 DustCloud Effect 생성
	CEffectManager::CreateEffect(EffectType::DUSTCLOUD, Pos, true);
	
}

void CPlayerIdletoRunState::Exit()
{
	// IdleToRun Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"PlayerIdleToRun")->Reset();
}
