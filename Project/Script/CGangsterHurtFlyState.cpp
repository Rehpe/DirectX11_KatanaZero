#include "pch.h"
#include "CGangsterHurtFlyState.h"

#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>
#include <Engine\CLevel.h>

#include "CEnemyScript.h"
#include "CEffectManager.h"
#include "CPlayerScript.h"

CGangsterHurtFlyState::CGangsterHurtFlyState()
{
	SetName(L"HurtFly");
}

CGangsterHurtFlyState::~CGangsterHurtFlyState()
{
}

void CGangsterHurtFlyState::finaltick()
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

void CGangsterHurtFlyState::Enter()
{
	CLevel* pLevel = CLevelManager::GetInst()->GetCurLevel();
	CGameObject* pPlayer = pLevel->FindObjectByName(L"Player");
	CPlayerScript* PlayerScript = pPlayer->GetScript<CPlayerScript>();

	// GangsterHurtFly Anim Àç»ý
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"GangsterHurtFly", false);

	CGameObject* Enemy = GetOwner();
	CEnemyScript* EnemyScript = dynamic_cast<CEnemyScript*>(GetOwner()->GetScripts()[0]);

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
	Ptr<CSound> EnemyDie = CResourceManager::GetInst()->FindRes<CSound>(L"sound\\SFX\\EnemyDead.wav");
	EnemyDie->Play(1, CTimeManager::GetInst()->GetTimeFactor(), 0.5f, false);
}

void CGangsterHurtFlyState::Exit()
{
	// GangsterHurtFly Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"GangsterHurtFly")->Reset();
}
