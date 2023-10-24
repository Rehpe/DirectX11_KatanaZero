#include "pch.h"
#include "CPlayerRunState.h"

#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>

#include <Engine\CKeyManager.h>
#include <Engine\CRigidBody2D.h>

#include "CScriptManager.h"
#include "CPlayerScript.h"

CPlayerRunState::CPlayerRunState()
{
}

CPlayerRunState::~CPlayerRunState()
{
}

void CPlayerRunState::finaltick()
{
	CGameObject* pPlayer = GetOwner();
	CPlayerScript* pPlayerScript = pPlayer->GetScript<CPlayerScript>();

	bool IsGround = pPlayer->RigidBody2D()->IsGround();
	bool IsOnSlope = pPlayer->RigidBody2D()->IsOnSlope();

	// Jump
	if ((!IsGround && !IsOnSlope) && pPlayer->RigidBody2D()->GetVelocity().y > 0)//pPlayerScript->IsJump() &&)
	{
		pPlayerScript->SetJump(true);
		GetOwnerFSM()->ChangeState(L"PlayerJump");
		return;
	}

	// Crouch
	if ((IsGround || IsOnSlope) && KEY_PRESSED(KEY::S))//pPlayerScript->GetInputY() < 0)
	{
		GetOwnerFSM()->ChangeState(L"PlayerPreCrouch");
		return;
	}

	// Roll
	if (abs(pPlayerScript->GetInputX()) >= 1 && (IsGround || IsOnSlope) && KEY_PRESSED(KEY::S))
	{
		// S키와 방향키를 동시에 눌렀을 경우 Roll
		GetOwnerFSM()->ChangeState(L"PlayerRoll");
		return;
	}


	// Fall

	// IdleToRun (방향전환)

	// RunToIdle
	if (abs(pPlayer->RigidBody2D()->GetVelocity().x) < 10.f && !(pPlayerScript->IsInput()))
	{
		// x축 가속도가 30 이하 + Move Input이 없다면
		GetOwnerFSM()->ChangeState(L"PlayerRunToIdle");
	}
}

void CPlayerRunState::Enter()
{
	// Run Anim 재생
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"PlayerRun", true);

	// Sound Effect
	Ptr<CSound> RunSFX = CResourceManager::GetInst()->FindRes<CSound>(L"sound\\SFX\\Player_Run1.wav");
	RunSFX->Play(1, CTimeManager::GetInst()->GetTimeFactor(), 0.5f, false);
}

void CPlayerRunState::Exit()
{
	// Run Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"PlayerRun")->Reset();
}
