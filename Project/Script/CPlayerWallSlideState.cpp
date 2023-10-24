#include "pch.h"
#include "CPlayerWallSlideState.h"

#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>

#include <Engine\CKeyManager.h>
#include <Engine\CTimeManager.h>
#include <Engine\CRigidBody2D.h>

#include "CScriptManager.h"

#include "CPlayerScript.h"

CPlayerWallSlideState::CPlayerWallSlideState()
	: m_fAntiGravityTime(0.3f)
{
	SetName(L"PlayerWallSlide");
}

CPlayerWallSlideState::~CPlayerWallSlideState()
{
}

void CPlayerWallSlideState::finaltick()
{
	CGameObject* pPlayer = GetOwner();
	CPlayerScript* pPlayerScript = pPlayer->GetScript<CPlayerScript>();

	static float AnitGravityCoolTime = 0.f;
	AnitGravityCoolTime += DT;

	if (AnitGravityCoolTime < m_fAntiGravityTime)
	{
		pPlayer->RigidBody2D()->SetGravity(false);
	}
	else
	{
		pPlayer->RigidBody2D()->SetGravity(true);
	}

	if (pPlayer->RigidBody2D()->IsGround() || pPlayer->RigidBody2D()->IsOnSlope())
	{
		// 땅으로 떨어지면 Idle
		pPlayer->RigidBody2D()->SetVelocity(Vec3(0, 0, 0));
	pPlayerScript->SetWallGrab(false);
	GetOwnerFSM()->ChangeState(L"PlayerIdle");
	return;
}
	if (KEY_TAP(KEY::W))
	{
		// Slide 상태에서 위 방향키(W) 누르면 반대쪽 벽으로 Flip
		pPlayerScript->SetWallGrab(false);
		GetOwnerFSM()->ChangeState(L"PlayerFlip");
		return;
	}

}

void CPlayerWallSlideState::Enter()
{
	// WallSlide Anim 재생
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"PlayerWallSlide", true);
	
	CGameObject* pPlayer = GetOwner();
	CPlayerScript* pPlayerScript = pPlayer->GetScript<CPlayerScript>();

	const int* WallCollision = pPlayerScript->GetWallCollision();
	if (WallCollision[(UINT)WALL_TYPE::HANGABLEWALL_LEFT])
	{
		pPlayerScript->SetLookDir(-1);
	}
	else if (WallCollision[(UINT)WALL_TYPE::HANGABLEWALL_RIGHT])
	{
		pPlayerScript->SetLookDir(1);
	}

	// Sound Effect
	Ptr<CSound> WallSlideSFX = CResourceManager::GetInst()->FindRes<CSound>(L"sound\\SFX\\Player_WallSlide.wav");
	WallSlideSFX->Play(1, CTimeManager::GetInst()->GetTimeFactor(), 0.5f, false);
}

void CPlayerWallSlideState::Exit()
{
	// WallSlide Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"PlayerWallSlide")->Reset();

}
