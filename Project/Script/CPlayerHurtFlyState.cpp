#include "pch.h"
#include "CPlayerHurtFlyState.h"
#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>

#include <Engine\CKeyManager.h>
#include <Engine\CRigidBody2D.h>
#include <Engine\CRecordManager.h>

#include "CScriptManager.h"
#include "CPlayerScript.h"

CPlayerHurtFlyState::CPlayerHurtFlyState()
{
	SetName(L"PlayerHurtFly");
}

CPlayerHurtFlyState::~CPlayerHurtFlyState()
{
}

void CPlayerHurtFlyState::finaltick()
{
	CGameObject* pPlayer = GetOwner();
	CPlayerScript* pPlayerScript = pPlayer->GetScript<CPlayerScript>();

	if(!GetOwner()->Animator2D()->GetCurAnim()->IsFinish())
		GetOwner()->RigidBody2D()->AddVelocity(Vec3(pPlayerScript->GetKnockBackDir().x * 200.f, 300.f, 0.f));

	if ( pPlayer->RigidBody2D()->GetVelocity().y <= 0 && (pPlayer->RigidBody2D()->IsGround() || pPlayer->RigidBody2D()->IsOnSlope()))
	{
		GetOwnerFSM()->ChangeState(L"PlayerHurtGround");
	}
}

void CPlayerHurtFlyState::Enter()
{
	CGameObject* pPlayer = GetOwner();
	CPlayerScript* pPlayerScript = pPlayer->GetScript<CPlayerScript>();

	// PlayerHurtFly Anim Àç»ý
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"PlayerHurtFly", false);
	if (!(Vec3(0.f, 0.f, 0.f) == pPlayerScript->GetKnockBackDir()))
	{
		pPlayerScript->SetDead(true);
		float fForce = pPlayerScript->GetKnockBackForce();
		GetOwner()->RigidBody2D()->SetGround(false);
	}

	// Sound Effect
	if (CRecordManager::GetInst()->GetPlayMode() != (UINT)PLAYMODE::NONE)
		return;

	Ptr<CSound> DieSFX = CResourceManager::GetInst()->FindRes<CSound>(L"sound\\SFX\\Player_Die.wav");
	DieSFX->Play(1, CTimeManager::GetInst()->GetTimeFactor(), 0.5f, false);
}

void CPlayerHurtFlyState::Exit()
{
	// PlayerHurtFly Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"PlayerHurtFly")->Reset();
}
