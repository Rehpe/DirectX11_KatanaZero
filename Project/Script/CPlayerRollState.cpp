#include "pch.h"
#include "CPlayerRollState.h"

#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>

#include <Engine\CKeyManager.h>
#include <Engine\CRigidBody2D.h>

#include "CScriptManager.h"
#include "CPlayerScript.h"

CPlayerRollState::CPlayerRollState()
{
	SetName(L"PlayerRoll");
}

CPlayerRollState::~CPlayerRollState()
{
}

void CPlayerRollState::finaltick()
{
	CGameObject* pPlayer = GetOwner();
	CPlayerScript* pPlayerScript = pPlayer->GetScript<CPlayerScript>();
	//pPlayer->RigidBody2D()->AddVelocity(Vec3(1000.f * pPlayerScript->GetLookDir(), 0.f, 0.f));
	pPlayer->RigidBody2D()->SetVelocityX(1000.f * pPlayerScript->GetLookDir());
	pPlayerScript->CreateShadow();

	if (GetOwnerFSM()->GetOwner()->Animator2D()->GetCurAnim()->IsFinish())
	{
		if(pPlayer->RigidBody2D()->GetVelocity().x > 0)
			GetOwnerFSM()->ChangeState(L"PlayerRun");
		else
			GetOwnerFSM()->ChangeState(L"PlayerRunToIdle");
	}
}

void CPlayerRollState::Enter()
{
	// Roll Anim Àç»ý
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"PlayerRoll", false);
	CGameObject* pPlayer = GetOwner();
	CPlayerScript* pPlayerScript = pPlayer->GetScript<CPlayerScript>();
	pPlayerScript->SetRoll(true);
	pPlayer->RigidBody2D()->SetVelocityLimitX(1000.f);

	// Sound Effect
	Ptr<CSound> RollSFX = CResourceManager::GetInst()->FindRes<CSound>(L"sound\\SFX\\Player_Roll.wav");
	RollSFX->Play(1, CTimeManager::GetInst()->GetTimeFactor(), 0.5f, false);
	
}

void CPlayerRollState::Exit()
{
	// Roll Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"PlayerRoll")->Reset();
	CGameObject* pPlayer = GetOwner();
	CPlayerScript* pPlayerScript = pPlayer->GetScript<CPlayerScript>();
	pPlayerScript->SetRoll(false);
	pPlayer->RigidBody2D()->SetVelocityLimitX(400.f);
}
