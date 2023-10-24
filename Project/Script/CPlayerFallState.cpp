#include "pch.h"
#include "CPlayerFallState.h"

#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>

#include <Engine\CKeyManager.h>
#include <Engine\CRigidBody2D.h>

#include "CScriptManager.h"
#include "CPlayerScript.h"
#include "CEffectManager.h"

CPlayerFallState::CPlayerFallState()
{
	SetName(L"PlayerFall");
}

CPlayerFallState::~CPlayerFallState()
{
}

void CPlayerFallState::finaltick()
{
	CGameObject* pPlayer = GetOwner();
	CPlayerScript* pPlayerScript = pPlayer->GetScript<CPlayerScript>();

	if (pPlayer->RigidBody2D()->IsGround() || pPlayer->RigidBody2D()->IsOnSlope())
	{
		pPlayerScript->SetFall(false);
		GetOwnerFSM()->ChangeState(L"PlayerIdle");

		Vec3 PlayerPos = GetOwner()->Transform()->GetRelativePos();
		Vec3 PlayerScale = GetOwner()->Collider2D()->GetOffsetScale();
		Vec3 Pos = PlayerPos - Vec3(0.f, PlayerScale.y / 2.f, 0.f);
		CEffectManager::CreateEffect(EffectType::LANDCLOUD, Pos, true);
		// Sound Effect
		Ptr<CSound> LandSFX = CResourceManager::GetInst()->FindRes<CSound>(L"sound\\SFX\\Player_Land.wav");
		LandSFX->Play(1, CTimeManager::GetInst()->GetTimeFactor(), 0.5f, false);

		return;
	}
}

void CPlayerFallState::Enter()
{
	// Fall Anim Àç»ý
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"PlayerFall", true);
}

void CPlayerFallState::Exit()
{
	// Fall Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"PlayerFall")->Reset();
}
