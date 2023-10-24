#include "pch.h"
#include "CEnemyDetectorScript.h"

#include <Engine\CRecordManager.h>
#include "CEnemyScript.h"

CEnemyDetectorScript::CEnemyDetectorScript()
	: CScript((UINT)SCRIPT_TYPE::ENEMYDETECTORSCRIPT)
	, m_fDetectRange(330.f)
{
}

CEnemyDetectorScript::~CEnemyDetectorScript()
{
}

void CEnemyDetectorScript::begin()
{
	if ((UINT)PLAYMODE::NONE != CRecordManager::GetInst()->GetPlayMode())
		return;

	// ÁÖÀÎ EnemyÀÇ À§Ä¡
	Vec3 EnemyPos = GetOwner()->GetParent()->Transform()->GetRelativePos();

	// DetectCollider
	GetOwner()->Collider2D()->SetAbsolute(true);
	GetOwner()->Collider2D()->SetOffsetScale(Vec2(m_fDetectRange, 5.f));
	GetOwner()->Transform()->SetAbsolute(true);
	//GetOwner()->Transform()->SetRelativePos(Vec3(EnemyPos.x - m_fDetectRange / 2.f, EnemyPos.y, 0.f));

}

void CEnemyDetectorScript::tick()
{
	if ((UINT)PLAYMODE::NONE != CRecordManager::GetInst()->GetPlayMode())
		return;

	// EnemyScript
	CEnemyScript* EnemyScript = dynamic_cast<CEnemyScript*>(GetOwner()->GetParent()->GetScripts()[0]);
	GetOwner()->Transform()->SetRelativePos(Vec3((m_fDetectRange / 2.f), 0.f, 0.f));
}

void CEnemyDetectorScript::BeginOverlap(CCollider2D* _Other)
{
	// ´ÝÈù ¹® °¨Áö ½Ã
	if (L"Door" == _Other->GetOwner()->GetLayerName())
	{
		CEnemyScript* EnemyScript = dynamic_cast<CEnemyScript*>(GetOwner()->GetParent()->GetScripts()[0]);
		if (EnemyScript == nullptr)
			return;
		EnemyScript->SetDetectingDoor(true);
	}
	// ´ÝÈù ¹® °¨Áö ½Ã
	if (L"LaserAware" == _Other->GetOwner()->GetLayerName())
	{
		CEnemyScript* EnemyScript = dynamic_cast<CEnemyScript*>(GetOwner()->GetParent()->GetScripts()[0]);
		if (EnemyScript == nullptr)
			return;
		EnemyScript->SetDetectingDoor(true);
	}
	// ÇÃ·¹ÀÌ¾î °¨Áö ½Ã
	if (L"Player" == _Other->GetOwner()->GetName())
	{
		CEnemyScript* EnemyScript = dynamic_cast<CEnemyScript*>(GetOwner()->GetParent()->GetScripts()[0]);
		if (EnemyScript == nullptr)
			return;
		EnemyScript->SetDetectingPlayer(true);
	}
	
}

void CEnemyDetectorScript::OnOverlap(CCollider2D* _Other)
{
	// ´ÝÈù ¹® °¨Áö ½Ã
	if (L"Door" == _Other->GetOwner()->GetLayerName())
	{
		CEnemyScript* EnemyScript = dynamic_cast<CEnemyScript*>(GetOwner()->GetParent()->GetScripts()[0]);
		if (EnemyScript == nullptr)
			return;
		EnemyScript->SetDetectingDoor(true);
	}
	// ´ÝÈù ¹® °¨Áö ½Ã
	if (L"LaserAware" == _Other->GetOwner()->GetLayerName())
	{
		CEnemyScript* EnemyScript = dynamic_cast<CEnemyScript*>(GetOwner()->GetParent()->GetScripts()[0]);
		if (EnemyScript == nullptr)
			return;
		EnemyScript->SetDetectingDoor(true);
	}
}


void CEnemyDetectorScript::EndOverlap(CCollider2D* _Other)
{
	//´ÝÈù ¹® °¨Áö ½Ã
	if (L"Door" == _Other->GetOwner()->GetLayerName())
	{
		CEnemyScript* EnemyScript = dynamic_cast<CEnemyScript*>(GetOwner()->GetParent()->GetScripts()[0]);
		if (EnemyScript == nullptr)
			return;
		EnemyScript->SetDetectingDoor(false);
	}
	//´ÝÈù ¹® °¨Áö ½Ã
	if (L"LaserAware" == _Other->GetOwner()->GetLayerName())
	{
		CEnemyScript* EnemyScript = dynamic_cast<CEnemyScript*>(GetOwner()->GetParent()->GetScripts()[0]);
		if (EnemyScript == nullptr)
			return;
		EnemyScript->SetDetectingDoor(false);
	}
	// ÇÃ·¹ÀÌ¾î °¨Áö ½Ã
	if (L"Player" == _Other->GetOwner()->GetName())
	{
		CEnemyScript* EnemyScript = dynamic_cast<CEnemyScript*>(GetOwner()->GetParent()->GetScripts()[0]);
		if (EnemyScript == nullptr)
			return;
		EnemyScript->SetDetectingPlayer(false);
	}
}
