#include "pch.h"
#include "CEffectManager.h"

#include <Engine\component.h>
#include <Engine\CLevel.h>
#include <Engine\CLayer.h>
#include <Engine\CPrefab.h>
#include <Engine\CResourceManager.h>

#include "CEffectScript.h"
#include "CPlayerScript.h"

void CEffectManager::CreateEffect(EffectType type, Vec3 pos, bool Destroy)
{
	CGameObject* Effect = nullptr;
	CEffectScript* EffectScript = nullptr;

	switch (type)
	{
	case EffectType::DUSTCLOUD:
	{
		Ptr<CPrefab> pDustCloud = CResourceManager::GetInst()->FindRes<CPrefab>(L"prefab\\Effect_DustCloud.pref");
		Effect = pDustCloud->Instantiate();
		Effect->GetScript<CEffectScript>()->SetEffectType((UINT)EffectType::DUSTCLOUD);
		Effect->GetScript<CEffectScript>()->SetPos(pos);
	}
	break;
	case EffectType::JUMPCLOUD:
	{
		Ptr<CPrefab> pJumpCloud = CResourceManager::GetInst()->FindRes<CPrefab>(L"prefab\\Effect_JumpCloud.pref");
		Effect = pJumpCloud->Instantiate();
		Effect->GetScript<CEffectScript>()->SetEffectType((UINT)EffectType::JUMPCLOUD);
		Effect->GetScript<CEffectScript>()->SetPos(pos);
	}
	break;
	case EffectType::LANDCLOUD:
	{
		Ptr<CPrefab> pLandCloud = CResourceManager::GetInst()->FindRes<CPrefab>(L"prefab\\Effect_LandCloud.pref");
		Effect = pLandCloud->Instantiate();
		Effect->GetScript<CEffectScript>()->SetEffectType((UINT)EffectType::LANDCLOUD);
		Effect->GetScript<CEffectScript>()->SetPos(pos);
	}
	break;
	case EffectType::BLOOD:
	{
		Ptr<CPrefab> pBlood = CResourceManager::GetInst()->FindRes<CPrefab>(L"prefab\\Blood.pref");
		Effect = pBlood->Instantiate();
		Effect->GetScript<CEffectScript>()->SetEffectType((UINT)EffectType::BLOOD);
		Effect->GetScript<CEffectScript>()->SetPos(pos);
	}
	break;
	case EffectType::BULLETREFLECT:
	{
		Ptr<CPrefab> pBulletRefelct = CResourceManager::GetInst()->FindRes<CPrefab>(L"prefab\\Effect_BulletReflect.pref");
		Effect = pBulletRefelct->Instantiate();
		Effect->GetScript<CEffectScript>()->SetEffectType((UINT)EffectType::BULLETREFLECT);
		Effect->GetScript<CEffectScript>()->SetPos(pos);
	}
	break;
	case EffectType::SLASH:
	{
		Ptr<CPrefab> pSlash = CResourceManager::GetInst()->FindRes<CPrefab>(L"prefab\\Effect_Slash.pref");
		Effect = pSlash->Instantiate();
		Effect->GetScript<CEffectScript>()->SetEffectType((UINT)EffectType::SLASH);
		Effect->GetScript<CEffectScript>()->SetPos(pos);
	}
	break;
	case EffectType::LONGSLASH:
	{
		Ptr<CPrefab> pLongSlash = CResourceManager::GetInst()->FindRes<CPrefab>(L"prefab\\Effect_LongSlash.pref");
		Effect = pLongSlash->Instantiate();
		Effect->GetScript<CEffectScript>()->SetEffectType((UINT)EffectType::LONGSLASH);
		CLevel* Level = CLevelManager::GetInst()->GetCurLevel();
		CPlayerScript* PlayerScript = Level->FindObjectByName(L"Player")->GetScript<CPlayerScript>();
		Effect->GetScript<CEffectScript>()->SetDir(PlayerScript->GetAttackDir());
		Effect->GetScript<CEffectScript>()->SetPos(pos);
	}
	break;
	}
	SpawnGameObject(Effect, pos, L"Effect");

}
