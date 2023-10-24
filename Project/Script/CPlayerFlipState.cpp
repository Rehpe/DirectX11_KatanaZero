#include "pch.h"
#include "CPlayerFlipState.h"

#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>

#include <Engine\CKeyManager.h>
#include <Engine\CRigidBody2D.h>

#include "CScriptManager.h"
#include "CPlayerScript.h"

CPlayerFlipState::CPlayerFlipState()
	: m_iPreviousWall(0)
{
	SetName(L"PlayerFlip");
}

CPlayerFlipState::~CPlayerFlipState()
{
}

void CPlayerFlipState::finaltick()
{
	CGameObject* pPlayer = GetOwner();
	CPlayerScript* pPlayerScript = pPlayer->GetScript<CPlayerScript>();
	pPlayerScript->CreateShadow();

	const int* WallCollision = pPlayerScript->GetWallCollision();
	int LookDir = pPlayerScript->GetLookDir();

	UINT OppositeWall = (m_iPreviousWall == (UINT)WALL_TYPE::HANGABLEWALL_LEFT) ? (UINT)WALL_TYPE::HANGABLEWALL_RIGHT : (UINT)WALL_TYPE::HANGABLEWALL_LEFT;

	if ( WallCollision[(UINT)OppositeWall])
	{
		pPlayerScript->SetFlip(false);
		pPlayerScript->SetLookDir((OppositeWall == (UINT)WALL_TYPE::HANGABLEWALL_LEFT) ? -1 : 1);
		GetOwnerFSM()->ChangeState(L"PlayerWallGrab");
		return;
	}
	if (pPlayerScript->IsOnGround() || pPlayerScript->IsOnSlope())
	{
		// 땅으로 떨어지면 Idle
		pPlayer->RigidBody2D()->SetVelocity(Vec3(0, 0, 0));
		pPlayerScript->SetFlip(false);
		GetOwnerFSM()->ChangeState(L"PlayerIdle");
		return;
	}
}

void CPlayerFlipState::Enter()
{
	// Flip Anim 재생
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"PlayerFlip", true);

	CGameObject* pPlayer = GetOwner();
	CPlayerScript* pPlayerScript = pPlayer->GetScript<CPlayerScript>();
	int LookDir = pPlayerScript->GetLookDir();

	pPlayerScript->SetFlip(true);

	float y = abs(sinf(XM_PI * 0.25f));
	float x = cosf(XM_PI * 0.25f);
	
	if (LookDir == -1)	// 왼쪽 벽에 붙어있음
	{
		Vec3 flyvector = Vec3(x, y, 0.f).Normalize();

		pPlayerScript->SetLookDir(1);
		pPlayer->RigidBody2D()->AddVelocity(flyvector * 500);
		m_iPreviousWall = (UINT)WALL_TYPE::HANGABLEWALL_LEFT;
	}
	else				// 오른쪽 벽에 붙어있음
	{
		Vec3 flyvector = Vec3(-x, y, 0.f).Normalize();
		pPlayerScript->SetLookDir(-1);
		pPlayer->RigidBody2D()->AddVelocity(flyvector * 500);
		m_iPreviousWall = (UINT)WALL_TYPE::HANGABLEWALL_RIGHT;
	}
	
}

void CPlayerFlipState::Exit()
{
	// Flip Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"PlayerFlip")->Reset();
	
	// State false
	CGameObject* pPlayer = GetOwner();
	CPlayerScript* pPlayerScript = pPlayer->GetScript<CPlayerScript>();
	pPlayerScript->SetFlip(false);
}
