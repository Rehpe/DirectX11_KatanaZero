#include "pch.h"
#include "CEnemyGangsterScript.h"

#include "CGangsterIdleState.h"
#include "CGangsterAimState.h"
#include "CGangsterAttackState.h"
#include "CGangsterWalkState.h"
#include "CGangsterTurnState.h"
#include "CGangsterHurtFlyState.h"
#include "CGangsterHurtGroundState.h"
#include "CGangsterRunState.h"

CEnemyGangsterScript::CEnemyGangsterScript()
	:CEnemyScript((UINT)SCRIPT_TYPE::ENEMYGANGSTERSCRIPT)
	, m_fShootTimer(0)
	, m_fShootCoolDown(5.f)
	, m_bIsShootable(true)
{
	AddScriptParam(SCRIPT_PARAM::VEC2, m_vSpawnPos, "Spawn Point");
	AddScriptParam(SCRIPT_PARAM::INT, &m_iNoticedDoorIdx, "DoorIdx");
}

CEnemyGangsterScript::~CEnemyGangsterScript()
{
}


void CEnemyGangsterScript::begin()
{
	CEnemyScript::begin();

	if (GetOwner()->FSM() == nullptr)
		return;

	// FSM에 State 추가
	GetOwner()->FSM()->AddState(L"Idle", new CGangsterIdleState);
	GetOwner()->FSM()->AddState(L"Aim", new CGangsterAimState);

	GetOwner()->FSM()->AddState(L"Attack", new CGangsterAttackState);
	GetOwner()->FSM()->AddState(L"Walk", new CGangsterWalkState);
	GetOwner()->FSM()->AddState(L"Turn", new CGangsterTurnState);
	GetOwner()->FSM()->AddState(L"HurtFly", new CGangsterHurtFlyState);
	GetOwner()->FSM()->AddState(L"HurtGround", new CGangsterHurtGroundState);
	GetOwner()->FSM()->AddState(L"Run", new CGangsterRunState);

	GetOwner()->FSM()->ChangeState(L"Walk");

	SetWalkSpeed(10.f);
}

void CEnemyGangsterScript::tick()
{
	CEnemyScript::tick();

	if (IsDead())
		return;

	// Shoot CoolDown 체크
	if (!m_bIsShootable)
	{
		m_fShootTimer += DT;
		if (m_fShootTimer >= m_fShootCoolDown)
		{
			m_bIsShootable = true;
			m_fShootTimer = 0.f;
		}
	}
}

void CEnemyGangsterScript::BeginOverlap(CCollider2D* _Other)
{
	CEnemyScript::BeginOverlap(_Other);
}

void CEnemyGangsterScript::OnOverlap(CCollider2D* _Other)
{
	CEnemyScript::OnOverlap(_Other);
}

void CEnemyGangsterScript::EndOverlap(CCollider2D* _Other)
{
	CEnemyScript::EndOverlap(_Other);
}

void CEnemyGangsterScript::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_vSpawnPos, sizeof(Vec3), 1, _File);
	fwrite(&m_iNoticedDoorIdx, sizeof(int), 1, _File);
}

void CEnemyGangsterScript::LoadFromLevelFile(FILE* _File)
{
	fread(&m_vSpawnPos, sizeof(Vec3), 1, _File);
	fread(&m_iNoticedDoorIdx, sizeof(int), 1, _File);
}
