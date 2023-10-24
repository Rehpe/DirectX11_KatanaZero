#include "pch.h"
#include "CLaserScript.h"

#include <Engine\CAnim2D.h>
#include <Engine\CLevel.h>
#include <Engine\CRecordManager.h>

#include "CEnemyScript.h"
#include "CPlayerScript.h"
#include "CGameManagerScript.h"

CLaserScript::CLaserScript()
	:CScript((UINT)SCRIPT_TYPE::LASERSCRIPT)
{
	
}

CLaserScript::~CLaserScript()
{
}

void CLaserScript::begin()
{
	GetOwner()->GetChildObjByName(L"LaserParticle")->
		GetRenderComponent()->SetMaterial(CResourceManager::GetInst()->FindRes<CMaterial>(L"ParticleTexRenderMtrl"));
	GetOwner()->GetChildObjByName(L"LaserParticle")->ParticleSystem()->SetParticleTex(CResourceManager::GetInst()->FindRes<CTexture>(L"texture\\particle\\Sparks.png"));
	tParticleModule Module = GetOwner()->GetChildObjByName(L"LaserParticle")->ParticleSystem()->GetModule();
	Module.ModuleCheck[(UINT)PARTICLE_MODULE::PARTICLE_SPAWN] = false;
	GetOwner()->GetChildObjByName(L"LaserParticle")->ParticleSystem()->SetModule(Module);
}

void CLaserScript::tick()
{

}

void CLaserScript::BeginOverlap(CCollider2D* _Other)
{
	if (L"Player" == _Other->GetOwner()->GetName())
	{
		CGameObject* Player = _Other->GetOwner();
		CPlayerScript* PlayerScript = Player->GetScript<CPlayerScript>();
		CLevel* pCurLevel = CLevelManager::GetInst()->GetCurLevel();
		CGameManagerScript* GMScript = pCurLevel->FindObjectByName(L"GameManager")->GetScript<CGameManagerScript>();

		if (PlayerScript->IsDead() || PlayerScript->IsRoll() || PlayerScript->IsFlip() || GMScript->IsCheatMode()
			|| CRecordManager::GetInst()->GetPlayMode() != (UINT)PLAYMODE::NONE)
			return;

		// Sound Effect
		Ptr<CSound> DieSFX = CResourceManager::GetInst()->FindRes<CSound>(L"sound\\SFX\\Player_Die.wav");
		DieSFX->Play(1, CTimeManager::GetInst()->GetTimeFactor(), 0.5f, false);

		// Sound Effect
		Ptr<CSound> LaserDead = CResourceManager::GetInst()->FindRes<CSound>(L"sound\\SFX\\LaserDead.wav");
		LaserDead->Play(1, CTimeManager::GetInst()->GetTimeFactor(), 0.5f, false);
	}

	// 적 / 플레이어 닿을시 사망
	if (L"Monster" == _Other->GetOwner()->GetLayerName())
	{
		CGameObject* Enemy = _Other->GetOwner();
		Vec3 EnemyPos = Enemy->Transform()->GetRelativePos();
		CEnemyScript* EnemyScript = Enemy->GetScript<CEnemyScript>();

		if (!EnemyScript->IsDead())
		{
			// Sound Effect
			Ptr<CSound> LaserDead = CResourceManager::GetInst()->FindRes<CSound>(L"sound\\SFX\\LaserDead.wav");
			LaserDead->Play(1, CTimeManager::GetInst()->GetTimeFactor(), 0.5f, false);
		}
	}	
}

void CLaserScript::OnOverlap(CCollider2D* _Other)
{
	// 적 / 플레이어 닿을시 사망
	if (L"Monster" == _Other->GetOwner()->GetLayerName())
	{
		tParticleModule Module = GetOwner()->GetChildObjByName(L"LaserParticle")->ParticleSystem()->GetModule();
		Module.ModuleCheck[(UINT)PARTICLE_MODULE::PARTICLE_SPAWN] = true;
		GetOwner()->GetChildObjByName(L"LaserParticle")->ParticleSystem()->SetModule(Module);

		CGameObject* Enemy = _Other->GetOwner();
		Vec3 EnemyPos = Enemy->Transform()->GetRelativePos();
		CEnemyScript* EnemyScript = Enemy->GetScript<CEnemyScript>();

		//Enemy->FSM()->ChangeState(L"HurtFly");
		Enemy->Animator2D()->Pause();
		int UseOverlay = 1;
		Enemy->MeshRender()->GetMaterial()->SetScalarParam(INT_1, &UseOverlay);
		Vec4 YellowOverlay = Vec4(1.f, 1.f, 0.f, 1.f);
		Enemy->MeshRender()->GetMaterial()->SetScalarParam(VEC4_0, &YellowOverlay);
		float OverlayAlpha = 0.8;
		Enemy->MeshRender()->GetMaterial()->SetScalarParam(FLOAT_0, &OverlayAlpha);
		Enemy->RigidBody2D()->SetVelocity(Vec3(0.f, 0.f, 0.f));
		Vec2 CurAnimSliceUV = Enemy->Animator2D()->GetCurAnim()->GetCurFrmSliceUV();
		EnemyScript->ReduceHp(1);
		EnemyScript->SetDead(true);
		Vec2 NewUV = Vec2(CurAnimSliceUV.x, CurAnimSliceUV.y - DT * 0.8f);
		if (NewUV.y <= 0)
		{
			NewUV.y = 0;
			Enemy->FSM()->ChangeState(L"HurtGround");
			//Enemy->Collider2D()->SetColliderActivate(false);
			tParticleModule Module = GetOwner()->GetChildObjByName(L"LaserParticle")->ParticleSystem()->GetModule();
			Module.ModuleCheck[(UINT)PARTICLE_MODULE::PARTICLE_SPAWN] = false;
			GetOwner()->GetChildObjByName(L"LaserParticle")->ParticleSystem()->SetModule(Module);
			GetOwner()->GetChildObjByName(L"LaserParticle")->Transform()->SetRelativePos(EnemyPos);
			int UseOverlay = 0;
			Enemy->MeshRender()->GetMaterial()->SetScalarParam(INT_1, &UseOverlay);
		}
		Enemy->Animator2D()->GetCurAnim()->SetCurFrmSliceUV(NewUV);
	}

	// Player와 충돌하면 Player Hp 감소
	if (L"Player" == _Other->GetOwner()->GetName())
	{
		CGameObject* Player = _Other->GetOwner();
		CPlayerScript* PlayerScript = Player->GetScript<CPlayerScript>();
		CLevel* pCurLevel = CLevelManager::GetInst()->GetCurLevel();
		CGameManagerScript* GMScript = pCurLevel->FindObjectByName(L"GameManager")->GetScript<CGameManagerScript>();

		if (PlayerScript->IsDead() || PlayerScript->IsRoll() || PlayerScript->IsFlip() || GMScript->IsCheatMode()
			 || CRecordManager::GetInst()->GetPlayMode() != (UINT)PLAYMODE::NONE)
			return;

		// 레이저 노란 파티클
		tParticleModule Module = GetOwner()->GetChildObjByName(L"LaserParticle")->ParticleSystem()->GetModule();
		Module.ModuleCheck[(UINT)PARTICLE_MODULE::PARTICLE_SPAWN] = true;
		GetOwner()->GetChildObjByName(L"LaserParticle")->ParticleSystem()->SetModule(Module);
		Vec3 PlayerPos = Player->Transform()->GetRelativePos();
		GetOwner()->GetChildObjByName(L"LaserParticle")->Transform()->SetRelativePos(PlayerPos);
			
		// 플레이어 멈춤
		Player->Animator2D()->Pause();
		Player->RigidBody2D()->SetVelocity(Vec3(0.f, 0.f, 0.f));
		PlayerScript->ReduceHp(1);
		PlayerScript->SetDead(true);

		// 플레이어 노란색 오버레이
		int UseOverlay = 1;
		Player->MeshRender()->GetMaterial()->SetScalarParam(INT_1, &UseOverlay);
		Vec4 YellowOverlay = Vec4(1.f, 1.f, 0.f, 1.f);
		Player->MeshRender()->GetMaterial()->SetScalarParam(VEC4_0, &YellowOverlay);
		float OverlayAlpha = 0.8;
		Player->MeshRender()->GetMaterial()->SetScalarParam(FLOAT_0, &OverlayAlpha);

		Vec2 CurAnimSliceUV = Player->Animator2D()->GetCurAnim()->GetCurFrmSliceUV();
		Vec2 NewUV = Vec2(CurAnimSliceUV.x, CurAnimSliceUV.y - DT * 2.f);
		if (NewUV.y <= 0)
		{
			NewUV.y = 0;

			// 레이저 파티클 Off
			tParticleModule Module = GetOwner()->GetChildObjByName(L"LaserParticle")->ParticleSystem()->GetModule();
			Module.ModuleCheck[(UINT)PARTICLE_MODULE::PARTICLE_SPAWN] = false;
			GetOwner()->GetChildObjByName(L"LaserParticle")->ParticleSystem()->SetModule(Module);
			
			// 플레이어 오버레이 끄기
			int UseOverlay = 0;
			Player->MeshRender()->GetMaterial()->SetScalarParam(INT_1, &UseOverlay);

			PlayerScript->ReduceHp(1);
			PlayerScript->SetDead(true);
		}
		Player->Animator2D()->GetCurAnim()->SetCurFrmSliceUV(NewUV);
	}
}
