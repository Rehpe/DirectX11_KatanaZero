#include "pch.h"
#include "CGangsterAimState.h"

#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>

#include "CEnemyScript.h"
#include "CGangsterBulletScript.h"
#include "CEnemyGangsterScript.h"

CGangsterAimState::CGangsterAimState()
{
	SetName(L"Aim");
}

CGangsterAimState::~CGangsterAimState()
{
}

void CGangsterAimState::finaltick()
{
	GetOwner()->RigidBody2D()->SetVelocity(Vec3(0.f, 0.f, 0.f));

	if (GetOwnerFSM()->GetOwner()->Animator2D()->GetCurAnim()->IsFinish())
	{
		GetOwnerFSM()->ChangeState(L"Idle");
	}
}

void CGangsterAimState::Enter()
{
	// GangsterAim Anim 재생
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"GangsterAim", false);

	CGameObject* pEnemy = GetOwner();
	CEnemyGangsterScript* GangsterScript = dynamic_cast<CEnemyGangsterScript*>(GetOwner()->GetScripts()[0]);
	GangsterScript->SetShootable(false);

	Ptr<CPrefab> pBulletPrefab = CResourceManager::GetInst()->FindRes<CPrefab>(L"prefab\\GangsterBullet.pref");
	CGameObject* pCloneBullet = pBulletPrefab->Instantiate();
	CGangsterBulletScript* BulletScript = pCloneBullet->GetScript<CGangsterBulletScript>();
	CGameObject* Gangster = this->GetOwner();
	BulletScript->SetShooter(Gangster);

	// 레벨에 추가
	SpawnGameObject(pCloneBullet, Gangster->Transform()->GetRelativePos(), L"MonsterProjectile");

	// Sound Effect
	Ptr<CSound> GunFireSFX = CResourceManager::GetInst()->FindRes<CSound>(L"sound\\SFX\\GunFire.wav");
	GunFireSFX->Play(1, CTimeManager::GetInst()->GetTimeFactor(), 0.5f, false);
}

void CGangsterAimState::Exit()
{
	// GangsterAim Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"GangsterAim")->Reset();
}
