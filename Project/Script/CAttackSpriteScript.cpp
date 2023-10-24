#include "pch.h"
#include "CAttackSpriteScript.h"

#include <Engine\CAnim2D.h>
#include <Engine\CLevelManager.h>
#include <Engine\CLevel.h>

#include "CEnemyScript.h"
#include "CEffectManager.h"
#include "CCameraScript.h"

CAttackSpriteScript::CAttackSpriteScript()
	: CScript((UINT)SCRIPT_TYPE::ATTACKSPRITESCRIPT)
{
}

CAttackSpriteScript::~CAttackSpriteScript()
{
}

void CAttackSpriteScript::tick()
{
	GetOwner()->Animator2D()->SetRepeat(false);

	if (GetOwner()->Animator2D()->GetCurAnim()->IsFinish())
	{
		Destroy();
		return;
	}

	CLevel* pLevel = CLevelManager::GetInst()->GetCurLevel();
	CGameObject* pPlayer = pLevel->FindObjectByName(L"Player");
	if (pPlayer == nullptr)
		return;
	Vec3 PlayerPos = pPlayer->Transform()->GetRelativePos();
	
	GetOwner()->Transform()->SetRelativePos(PlayerPos);
}

void CAttackSpriteScript::BeginOverlap(CCollider2D* _Other)
{
	if (L"Monster" == _Other->GetOwner()->GetLayerName())
	{

		CGameObject* Enemy = _Other->GetOwner();
		Vec3 EnemyPos = Enemy->Transform()->GetRelativePos();
		CEnemyScript* EnemyScript = Enemy->GetScript<CEnemyScript>();

		if (EnemyScript->IsDead())
			return;

		CEffectManager::CreateEffect(EffectType::SLASH, EnemyPos, true);
		CEffectManager::CreateEffect(EffectType::LONGSLASH, EnemyPos, true);

		CLevel* pLevel = CLevelManager::GetInst()->GetCurLevel();
		CGameObject* Camera = pLevel->FindObjectByName(L"MainCamera");
		CCameraScript* CamScript = Camera->GetScript<CCameraScript>();
		CamScript->ApplyCameraShake(0.5f, 40.f);

		EnemyScript->CalcKnockBackDir(this->Collider2D());
		EnemyScript->ReduceHp(1);
	}
}
