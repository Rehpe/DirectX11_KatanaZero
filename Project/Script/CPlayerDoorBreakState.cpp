#include "pch.h"
#include "CPlayerDoorBreakState.h"

#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>
#include <Engine\CLevel.h>

#include <Engine\CKeyManager.h>
#include <Engine\CRigidBody2D.h>

#include "CScriptManager.h"
#include "CPlayerScript.h"
#include "CCameraScript.h"

CPlayerDoorBreakState::CPlayerDoorBreakState()
{
	SetName(L"PlayerDoorBreak");
}

CPlayerDoorBreakState::~CPlayerDoorBreakState()
{
}

void CPlayerDoorBreakState::finaltick()
{
	CGameObject* pPlayer = GetOwner();
	CPlayerScript* pPlayerScript = pPlayer->GetScript<CPlayerScript>();

	pPlayer->RigidBody2D()->SetVelocity(Vec3(0.f, 0.f, 0.f));

	if (GetOwner()->Animator2D()->GetCurAnim()->IsFinish())
	{
		GetOwnerFSM()->ChangeState(L"PlayerIdle");
	}
}

void CPlayerDoorBreakState::Enter()
{
	// DoorBreak Anim Àç»ý
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"PlayerDoorBreak", false);

	// Sound Effect
	Ptr<CSound> DoorBreakSFX = CResourceManager::GetInst()->FindRes<CSound>(L"sound\\SFX\\DoorOpen.wav");
	DoorBreakSFX->Play(1, CTimeManager::GetInst()->GetTimeFactor(), 0.5f, false);


	CLevel* pLevel = CLevelManager::GetInst()->GetCurLevel();
	CGameObject* Camera = pLevel->FindObjectByName(L"MainCamera");
	CCameraScript* CamScript = Camera->GetScript<CCameraScript>();
	CamScript->ApplyCameraShake(0.5f, 40.f);
}

void CPlayerDoorBreakState::Exit()
{
	// DoorBreak Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"PlayerDoorBreak")->Reset();
}
