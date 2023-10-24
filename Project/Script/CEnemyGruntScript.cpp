#include "pch.h"
#include "CEnemyGruntScript.h"

#include "CGruntIdleState.h"
#include "CGruntRunState.h"
#include "CGruntAttackState.h"
#include "CGruntWalkState.h"
#include "CGruntTurnState.h"
#include "CGruntHurtFlyState.h"
#include "CGruntHurtGroundState.h"

CEnemyGruntScript::CEnemyGruntScript()
	:CEnemyScript((UINT)SCRIPT_TYPE::ENEMYGRUNTSCRIPT)
{
	//AddScriptParam(SCRIPT_PARAM::VEC2, m_vSpawnPos, "Spawn Point");
	//AddScriptParam(SCRIPT_PARAM::INT, &m_iNoticedDoorIdx, "DoorIdx");
}

CEnemyGruntScript::~CEnemyGruntScript()
{
}

void CEnemyGruntScript::begin()
{
	CEnemyScript::begin();

	if (GetOwner()->FSM() == nullptr)
		return;

	// FSM¿¡ State Ãß°¡
	GetOwner()->FSM()->AddState(L"Idle", new CGruntIdleState);
	GetOwner()->FSM()->AddState(L"Run", new CGruntRunState);
	GetOwner()->FSM()->AddState(L"Attack", new CGruntAttackState);
	GetOwner()->FSM()->AddState(L"Walk", new CGruntWalkState);
	GetOwner()->FSM()->AddState(L"Turn", new CGruntTurnState);
	GetOwner()->FSM()->AddState(L"HurtFly", new CGruntHurtFlyState);
	GetOwner()->FSM()->AddState(L"HurtGround", new CGruntHurtGroundState);
	
	GetOwner()->FSM()->ChangeState(L"Walk");

	SetWalkSpeed(10.f);
}

void CEnemyGruntScript::tick()
{
	CEnemyScript::tick();
}

void CEnemyGruntScript::BeginOverlap(CCollider2D* _Other)
{
	CEnemyScript::BeginOverlap(_Other);
}

void CEnemyGruntScript::OnOverlap(CCollider2D* _Other)
{
	CEnemyScript::OnOverlap(_Other);
}

void CEnemyGruntScript::EndOverlap(CCollider2D* _Other)
{
	CEnemyScript::EndOverlap(_Other);
}

void CEnemyGruntScript::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_vSpawnPos, sizeof(Vec3), 1, _File);
	fwrite(&m_iNoticedDoorIdx, sizeof(int), 1, _File);
}

void CEnemyGruntScript::LoadFromLevelFile(FILE* _File)
{
	fread(&m_vSpawnPos, sizeof(Vec3), 1, _File);
	fread(&m_iNoticedDoorIdx, sizeof(int), 1, _File);
}
