#include "pch.h"
#include "CPlayerWallGrabState.h"

#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>

#include <Engine\CKeyManager.h>
#include <Engine\CRigidBody2D.h>

#include "CScriptManager.h"

#include "CPlayerScript.h"

CPlayerWallGrabState::CPlayerWallGrabState()
	: m_fAntiGravityTime(0.3f)
{
	SetName(L"PlayerWallGrab");
}

CPlayerWallGrabState::~CPlayerWallGrabState()
{
}

void CPlayerWallGrabState::finaltick()
{
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

	pPlayerScript->SetWallGrab(true);

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

	if (pPlayer->RigidBody2D()->GetVelocity(). y <= 0.01f)
	{
		// WallGrab ���¿��� �߷����� �������� ��� WallSlide
		GetOwnerFSM()->ChangeState(L"PlayerWallSlide");
		return;
	}
	if (KEY_TAP(KEY::W))
	{
		// Slide ���¿��� �� ����Ű(W) ������ �ݴ��� ������ Flip
		pPlayerScript->SetWallGrab(false);
		GetOwnerFSM()->ChangeState(L"PlayerFlip");
		return;
	}
	if (pPlayerScript->IsOnGround() || pPlayerScript->IsOnSlope())
	{
		// ������ �������� Idle
		pPlayer->RigidBody2D()->SetVelocity(Vec3(0, 0, 0));
		pPlayerScript->SetWallGrab(false);
		GetOwnerFSM()->ChangeState(L"PlayerIdle");
		return;
	}
}

void CPlayerWallGrabState::Enter()
{
	// WallGrab Anim ���
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"PlayerWallGrab", false);
	
	CGameObject* pPlayer = GetOwner();
	CPlayerScript* pPlayerScript = pPlayer->GetScript<CPlayerScript>();

	// ���� �ٴ� ���� �ణ �ö󰡰�
	
	if (pPlayer->RigidBody2D()->GetVelocity().y >= 0)
	{
		pPlayer->RigidBody2D()->AddVelocity(Vec3(0, 50.f, 0));
	}
	else
	{
		pPlayer->RigidBody2D()->SetVelocityY(50.f);
	}

	// Sound Effect
	Ptr<CSound> WallGrabSFX = CResourceManager::GetInst()->FindRes<CSound>(L"sound\\SFX\\Player_WallGrab1.wav");
	WallGrabSFX->Play(1, CTimeManager::GetInst()->GetTimeFactor(), 0.5f, false);
}

void CPlayerWallGrabState::Exit()
{
	// WallGrab Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"PlayerWallGrab")->Reset();
	CGameObject* pPlayer = GetOwner();
	CPlayerScript* pPlayerScript = pPlayer->GetScript<CPlayerScript>();
	pPlayerScript->SetWallGrab(false);
}
