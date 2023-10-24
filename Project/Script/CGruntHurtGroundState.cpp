#include "pch.h"
#include "CGruntHurtGroundState.h"
#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>

#include "CEnemyScript.h"
#include "CEffectManager.h"

CGruntHurtGroundState::CGruntHurtGroundState()
{
	SetName(L"GruntHurtGround");
}

CGruntHurtGroundState::~CGruntHurtGroundState()
{
}

void CGruntHurtGroundState::finaltick()
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

void CGruntHurtGroundState::Enter()
{
	// GruntHurtGround Anim Àç»ý
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"GruntHurtGround", false);

	// Sound Effect
	Ptr<CSound> EnemyDown = CResourceManager::GetInst()->FindRes<CSound>(L"sound\\SFX\\EnemyDown.wav");
	EnemyDown->Play(1, CTimeManager::GetInst()->GetTimeFactor(), 0.5f, false);
}

void CGruntHurtGroundState::Exit()
{
	// GruntHurtGround Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"GruntHurtGround")->Reset();
}
