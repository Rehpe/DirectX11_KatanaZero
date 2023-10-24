#include "pch.h"
#include "CGruntAttackSpriteScript.h"

#include <Engine\CAnim2D.h>
#include <Engine\CLevelManager.h>
#include <Engine\CLevel.h>

#include "CPlayerScript.h"
#include "CGameManagerScript.h"

CGruntAttackSpriteScript::CGruntAttackSpriteScript()
	: CScript((UINT)SCRIPT_TYPE::GRUNTATTACKSPRITESCRIPT)
	, m_bActivate(false)
{
}

CGruntAttackSpriteScript::~CGruntAttackSpriteScript()
{
}

void CGruntAttackSpriteScript::tick()
{
	//GetOwner()->Animator2D()->SetRepeat(false);

	if (!m_bActivate)
	{
		// 안보이게
		GetOwner()->Animator2D()->SetCurAnim(nullptr);

		// 충돌체 끔
		GetOwner()->Collider2D()->SetColliderActivate(false);

		return;
	}

	
	if (nullptr == GetOwner()->Animator2D()->GetCurAnim())
	{
		if (L"Grunt" == GetOwner()->GetParent()->GetName())
		{
			GetOwner()->Animator2D()->Play(L"GruntAttackSprite", false);
		}
		else if (L"Gangster" == GetOwner()->GetParent()->GetName())
		{
			GetOwner()->Animator2D()->Play(L"GangsterAttackSprite", false);
		}
		GetOwner()->Collider2D()->SetColliderActivate(true);
	}

	if (GetOwner()->Animator2D()->GetCurAnim()->IsFinish())
	{
		m_bActivate = false;
		if (L"Grunt" == GetOwner()->GetParent()->GetName())
		{
			GetOwner()->Animator2D()->FindAnim(L"GruntAttackSprite")->Reset();
		}
		else if (L"Gangster" == GetOwner()->GetParent()->GetName())
		{
			GetOwner()->Animator2D()->FindAnim(L"GangsterAttackSprite")->Reset();
		}
	}
	
}

void CGruntAttackSpriteScript::BeginOverlap(CCollider2D* _Other)
{
	if (L"Player" == _Other->GetOwner()->GetName())
	{
		CGameObject* Player = _Other->GetOwner();
		CPlayerScript* PlayerScript = Player->GetScript<CPlayerScript>();
		CLevel* pCurLevel = CLevelManager::GetInst()->GetCurLevel();
		CGameManagerScript* GMScript = pCurLevel->FindObjectByName(L"GameManager")->GetScript<CGameManagerScript>();

		if (PlayerScript->IsRoll() || PlayerScript->IsFlip() || GMScript->IsCheatMode())
			return;

		PlayerScript->CalcKnockBackDir(this->Collider2D());
		PlayerScript->ReduceHp(1);
	}
}

void CGruntAttackSpriteScript::OnOverlap(CCollider2D* _Other)
{
}

void CGruntAttackSpriteScript::EndOverlap(CCollider2D* _Other)
{
}
