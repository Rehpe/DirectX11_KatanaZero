#include "pch.h"
#include "CPlayerJumpState.h"

#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>

#include <Engine\CKeyManager.h>
#include <Engine\CRigidBody2D.h>

#include "CScriptManager.h"
#include "CPlayerScript.h"
#include "CEffectManager.h"

CPlayerJumpState::CPlayerJumpState()
{
	SetName(L"PlayerJump");
}

CPlayerJumpState::~CPlayerJumpState()
{
}

void CPlayerJumpState::finaltick()
{
	CGameObject* pPlayer = GetOwner();
	CPlayerScript* pPlayerScript = pPlayer->GetScript<CPlayerScript>();

	pPlayerScript->SetJump(true);

	if (pPlayer->RigidBody2D()->GetVelocity().y <= 0)
	{
		pPlayerScript->SetJump(false);
		pPlayerScript->SetFall(true);
		GetOwnerFSM()->ChangeState(L"PlayerFall");
		return;
	}
}

void CPlayerJumpState::Enter()
{
	// Jump Anim 犁积
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"PlayerJump", true);

	Vec3 PlayerPos = GetOwner()->Transform()->GetRelativePos();
	Vec3 PlayerScale = GetOwner()->Transform()->GetRelativeScale();
	Vec3 Pos = PlayerPos - Vec3(0.f, PlayerScale.y / 2.f, 0.f);
	// Player 惯 关俊 Jump Effect 积己
	CEffectManager::CreateEffect(EffectType::JUMPCLOUD, PlayerPos, true);
	// Sound Effect
	Ptr<CSound> JumpSFX = CResourceManager::GetInst()->FindRes<CSound>(L"sound\\SFX\\Player_Jump.wav");
	JumpSFX->Play(1, CTimeManager::GetInst()->GetTimeFactor(), 0.5f, false);
}

void CPlayerJumpState::Exit()
{
	// Jump Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"PlayerJump")->Reset();
}
