#include "pch.h"
#include "CGangsterHurtGroundState.h"

#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>

#include "CEnemyScript.h"
#include "CEffectManager.h"

CGangsterHurtGroundState::CGangsterHurtGroundState()
{
	SetName(L"HurtGround");
}

CGangsterHurtGroundState::~CGangsterHurtGroundState()
{
}

void CGangsterHurtGroundState::finaltick()
{
	static float BloodTimer = 0.f;
	BloodTimer += DT;

	CGameObject* Enemy = GetOwner();
	CEnemyScript* EnemyScript = dynamic_cast<CEnemyScript*>(GetOwner()->GetScripts()[0]);

	if (GetOwner()->Animator2D()->GetCurAnim()->IsFinish())
	{
		// Particle Off
		if (BloodTimer >= 1.5f)
		{
			tParticleModule Module = GetOwner()->GetChildObjByName(L"BloodParticle")->ParticleSystem()->GetModule();
			Module.ModuleCheck[(UINT)PARTICLE_MODULE::PARTICLE_SPAWN] = false;
			GetOwner()->GetChildObjByName(L"BloodParticle")->ParticleSystem()->SetModule(Module);
		}
	}
}

void CGangsterHurtGroundState::Enter()
{
	// GangsterHurtGround Anim Àç»ý
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"GangsterHurtGround", false);

	// Sound Effect
	Ptr<CSound> EnemyDown = CResourceManager::GetInst()->FindRes<CSound>(L"sound\\SFX\\EnemyDown.wav");
	EnemyDown->Play(1, CTimeManager::GetInst()->GetTimeFactor(), 0.5f, false);
}

void CGangsterHurtGroundState::Exit()
{
	// GangsterHurtGround Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"GangsterHurtGround")->Reset();
}
