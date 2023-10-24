#include "pch.h"
#include "CGruntHurtFlyState.h"
#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>
#include <Engine\CLevel.h>

#include "CEnemyScript.h"
#include "CEffectManager.h"
#include "CPlayerScript.h"

CGruntHurtFlyState::CGruntHurtFlyState()
{
	SetName(L"HurtFly");
}

CGruntHurtFlyState::~CGruntHurtFlyState()
{
}

void CGruntHurtFlyState::finaltick()
{
	CGameObject* Enemy = GetOwner();
	CEnemyScript* EnemyScript = dynamic_cast<CEnemyScript*>(GetOwner()->GetScripts()[0]);

	if (!GetOwner()->Animator2D()->GetCurAnim()->IsFinish())
	{
		GetOwner()->RigidBody2D()->AddVelocity(Vec3(EnemyScript->GetKnockBackDir().x * 200.f, 300.f, 0.f));
	}
	if (Enemy->RigidBody2D()->GetVelocity().y <= 0 && (Enemy->RigidBody2D()->IsGround() || Enemy->RigidBody2D()->IsOnSlope()))
	{
		GetOwnerFSM()->ChangeState(L"HurtGround");
	}
}

void CGruntHurtFlyState::Enter()
{
	// GruntHurtFly Anim Àç»ý
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"GruntHurtFly", true);

	CGameObject* Enemy = GetOwner();
	CEnemyScript* EnemyScript = dynamic_cast<CEnemyScript*>(GetOwner()->GetScripts()[0]);
	CLevel* pLevel = CLevelManager::GetInst()->GetCurLevel();
	CGameObject* pPlayer = pLevel->FindObjectByName(L"Player");
	CPlayerScript* PlayerScript = pPlayer->GetScript<CPlayerScript>();

	if (!(Vec3(0.f, 0.f, 0.f) == EnemyScript->GetKnockBackDir()))
	{
		EnemyScript->SetDead(true);
		GetOwner()->RigidBody2D()->SetGround(false);
	}

	//Particle On
	tParticleModule Module = GetOwner()->GetChildObjByName(L"BloodParticle")->ParticleSystem()->GetModule();
	Module.ModuleCheck[(UINT)PARTICLE_MODULE::PARTICLE_SPAWN] = true;
	Module.vVelocityDir = PlayerScript->GetAttackDir();
	GetOwner()->GetChildObjByName(L"BloodParticle")->ParticleSystem()->SetModule(Module);

	// Sound Effect
	Ptr<CSound> EnemyDead = CResourceManager::GetInst()->FindRes<CSound>(L"sound\\SFX\\EnemyDead.wav");
	EnemyDead->Play(1, CTimeManager::GetInst()->GetTimeFactor(), 0.5f, false);
}

void CGruntHurtFlyState::Exit()
{
	// GruntHurtFly Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"GruntHurtFly")->Reset();
}
