#include "pch.h"
#include "CFanScript.h"

#include "CPlayerScript.h"

CFanScript::CFanScript()
	: CScript((UINT)SCRIPT_TYPE::FANSCRIPT)
{
}

CFanScript::~CFanScript()
{
}


void CFanScript::begin()
{
	GetOwner()->Collider2D()->SetColliderActivate(true);
}

void CFanScript::tick()
{
	if(CTimeManager::GetInst()->GetTimeFactor() == 1.f)
		GetOwner()->Collider2D()->SetColliderActivate(true);
	else
		GetOwner()->Collider2D()->SetColliderActivate(false);
}

void CFanScript::OnOverlap(CCollider2D* _Other)
{
	if (L"Player" == _Other->GetOwner()->GetName())
	{
		
			CGameObject* Player = _Other->GetOwner();
			CPlayerScript* PlayerScript = Player->GetScript<CPlayerScript>();
			
			Player->FSM()->ChangeState(L"PlayerHurtFly");
			PlayerScript->ReduceHp(1);
			PlayerScript->SetDead(true);
		
	}
}
