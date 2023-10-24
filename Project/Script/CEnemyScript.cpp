#include "pch.h"
#include "CEnemyScript.h"
#include "CEnemyGruntScript.h"
#include "CWallScript.h"

#include <Engine\CLevel.h>
#include <Engine\CState.h>
#include <Engine\CLayer.h>
#include <Engine\CRecordManager.h>


CEnemyScript::CEnemyScript(UINT ScriptType)
: CScript(ScriptType)
, m_iHp(1)
, m_fSpeed(100)
, m_iLookDir(-1)
, m_bDead(false)
, m_bGround(false)
, m_fAttackCoolDown(1.f)
, m_bIsAttackable(true)
, m_bIsCollidingWithPlayer(false)
{
}

CEnemyScript::CEnemyScript()
	: CScript((UINT)SCRIPT_TYPE::ENEMYSCRIPT)
	, m_iHp(1)
	, m_fSpeed(5)
	, m_bDead(false)
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, &m_iNoticedDoorIdx, "Noticed Door Num");
}

CEnemyScript::~CEnemyScript()
{
}


void CEnemyScript::begin()
{
	// 동적 재질을 생성하고 현재 재질로 세팅
	MeshRender()->GetDynamicMaterial();

	GetOwner()->RigidBody2D()->SetGravity(true);
	GetOwner()->RigidBody2D()->SetVelocity(Vec3(0.f, 0.f, 0.f));
	m_iLookDir = -1;
	GetOwner()->Transform()->SetRelativePos(m_vSpawnPos);

	CGameObject* EnemyNoticedEffect = GetOwner()->GetChildObjByName(L"EnemyNoticed");
	EnemyNoticedEffect->Animator2D()->SetCurAnim(nullptr);
}

void CEnemyScript::tick()
{
	if ((UINT)PLAYMODE::NONE != CRecordManager::GetInst()->GetPlayMode())
		return;

	CLevel* pLevel = CLevelManager::GetInst()->GetCurLevel();
	CGameObject* pDetector = GetOwner()->GetChildObjByName(L"Detector");
	GetOwner()->RigidBody2D()->SetMovingInput(true);

	if (m_bNoticed)
	{
		CGameObject* EnemyNoticedEffect = GetOwner()->GetChildObjByName(L"EnemyNoticed");
		EnemyNoticedEffect->Animator2D()->Play(L"EnemyNoticed", false);
	}
		
	// 캐릭터 방향 Check
	if (m_iLookDir > 0)
		GetOwner()->Transform()->SetRelativeRot(0.f, 0.f, 0.f);
	else
		GetOwner()->Transform()->SetRelativeRot(0.f, XM_PI, 0.f);

	// 중력 Check
	if (m_bGround)
	{
		GetOwner()->RigidBody2D()->SetGround(true);
	}
	else
	{
		GetOwner()->RigidBody2D()->SetGround(false);
	}

	if (m_iHp <= 0)
	{
		GetOwner()->RigidBody2D()->SetMovingInput(false);
	}

	// Attack CoolDown 체크
	if (!m_bIsAttackable)
	{
		m_fAttackTimer += DT;
		if (m_fAttackTimer >= m_fAttackCoolDown)
		{
			m_bIsAttackable = true;
			m_fAttackTimer = 0.f;
		}
	}

	if (m_iHp <= 0 && !m_bDead)
	{
		//죽음
		m_bNoticed = false;
		CGameObject* EnemyNoticedEffect = GetOwner()->GetChildObjByName(L"EnemyNoticed");
		EnemyNoticedEffect->Animator2D()->SetCurAnim(nullptr);
		GetOwner()->FSM()->ChangeState(L"HurtFly");
		
	}

}

void CEnemyScript::BeginOverlap(CCollider2D* _Other)
{
	if (m_bDead)
		return;
	
	if (GetOwner()->FSM()->FindState(L"Walk") == nullptr)
		return;

	// 플레이어와 충돌 시
	if (L"Player" == _Other->GetOwner()->GetName())
	{
		m_bIsCollidingWithPlayer = true;
	}

	// Walk 상태에서 벽/문과 닿을 경우
	if (L"Walk" == GetOwner()->FSM()->GetCurState()->GetName())
	{
		if (L"Wall" == _Other->GetOwner()->GetLayerName())
		{
			// 방향 전환
			int prevLookDir = GetLookDir();
			SetLookDir(prevLookDir * -1);
			GetOwner()->FSM()->ChangeState(L"Turn");
		}

		if (L"Ramp" == _Other->GetOwner()->GetLayerName())
		{
			// 방향 전환
			int prevLookDir = GetLookDir();
			SetLookDir(prevLookDir * -1);
			GetOwner()->FSM()->ChangeState(L"Turn");
		}

		if (L"Door" == _Other->GetOwner()->GetLayerName())
		{
			// 방향 전환
			int prevLookDir = GetLookDir();
			SetLookDir(prevLookDir * -1);
			GetOwner()->FSM()->ChangeState(L"Turn");
		}

		if (L"LaserAware" == _Other->GetOwner()->GetLayerName())
		{
			// 방향 전환
			int prevLookDir = GetLookDir();
			SetLookDir(prevLookDir * -1);
			GetOwner()->FSM()->ChangeState(L"Turn");
		}
	}

	// Run 상태에서 LaserAware에 닿을 경우
	if (L"Run" == GetOwner()->FSM()->GetCurState()->GetName())
	{
		if (L"LaserAware" == _Other->GetOwner()->GetLayerName())
		{
			// 방향 전환
			int prevLookDir = GetLookDir();
			SetLookDir(prevLookDir * -1);
			GetOwner()->FSM()->ChangeState(L"Turn");
		}
	}

	// Run 상태 && 플레이어가 다른 층에 있을 때 경사로와 충돌한 경우
	if (L"Run" == GetOwner()->FSM()->GetCurState()->GetName() && !IsPlayerOnSameFloor())
	{
		if (L"Ramp" == _Other->GetOwner()->GetLayerName())
		{
			m_bIsUsingRamp = true;
			m_vCollidingRampStartPos = _Other->GetOwner()->Collider2D()->GetStartPos();
			m_vCollidingRampEndPos = _Other->GetOwner()->Collider2D()->GetEndPos();

			// 멈출 수 있게 Velocity 조절
			//GetOwner()->RigidBody2D()->SetVelocityX(0.f);

		}
	}
}


void CEnemyScript::OnOverlap(CCollider2D* _Other)
{

}

void CEnemyScript::EndOverlap(CCollider2D* _Other)
{
	if (m_bDead)
		return;

	// 플레이어 감지 시
	if (L"Player" == _Other->GetOwner()->GetName())
	{
		m_bIsCollidingWithPlayer = false;
	}

	// Run 상태 && 플레이어가 다른 층에 있을 때 경사로와 떨어진 경우
	if (L"Run" == GetOwner()->FSM()->GetCurState()->GetName())// && !IsPlayerOnSameFloor())
	{
		if (L"Ramp" == _Other->GetOwner()->GetLayerName())
		{
			m_bIsUsingRamp = false;
			m_vCollidingRampStartPos = Vec3(0.f, 0.f, 0.f);
			m_vCollidingRampEndPos = Vec3(0.f, 0.f, 0.f);
		}
	}


}

bool CEnemyScript::IsPlayerOnSameFloor()
{
	CLevel* pCurLevel = CLevelManager::GetInst()->GetCurLevel();
	CGameObject* Player = pCurLevel->FindObjectByName(L"Player");

	Vec3 PlayerPos = Player->Transform()->GetRelativePos();
	Vec3 EnemyPos = GetOwner()->Transform()->GetRelativePos();

	float fTolerance = 30.0f;
	return abs(PlayerPos.y - EnemyPos.y) < fTolerance;
}

Vec3 CEnemyScript::FindNearestRampPos()
{
	CLevel* pCurLevel = CLevelManager::GetInst()->GetCurLevel();
	CGameObject* Player = pCurLevel->FindObjectByName(L"Player");

	Vec3 PlayerPos = Player->Transform()->GetRelativePos();
	Vec3 EnemyPos = GetOwner()->Transform()->GetRelativePos();

	CLevel* pLevel = CLevelManager::GetInst()->GetCurLevel();
	vector<CGameObject*> ramps = pLevel->FindLayerByName(L"Ramp")->GetPreObjects();

	// 플레이어가 위층에 있다 -> 올라가야함(StartPos) 
	// 플레이어가 아래층에 있다-> 내려가야함(EndPos)
	Vec3 RampPos;
	Vec3 NearestRampPos;

	float fMinDistance = FLT_MAX;

	for (size_t i = 0; i < ramps.size(); i++)
	{
		if (EnemyPos.y > PlayerPos.y)
		{
			// 플레이어가 아래층에 있는 경우 -> 경사로 Collider에서 EndPos 찾아옴
			RampPos = ramps[i]->Collider2D()->GetEndPos();
		}
		else
		{
			//플레이어가 위층에 있는 경우 -> 경사로 Collider에서 StartPos 찾아옴
			RampPos = ramps[i]->Collider2D()->GetStartPos();
		}

		// 찾아온 좌표값과 Enemy의 현재 위치를 비교하여 가장 가까운 곳을 찾는다
		Vec3 Delta = RampPos - EnemyPos;
		float fDistance = Delta.Length();
		
		if (fDistance < fMinDistance)
		{
			fMinDistance = fDistance;
			NearestRampPos = RampPos;
		}
	}
	
	return NearestRampPos;
}

void CEnemyScript::CalcKnockBackDir(CCollider2D* _Other)
{
	Vec3 EnemyPos = GetOwner()->Transform()->GetRelativePos();
	Vec3 ProjectilePos = _Other->GetOwner()->Collider2D()->GetColliderWorldPos();
	Vec3 knockbackDirection = EnemyPos - ProjectilePos;
	knockbackDirection.y = 1.0f; // Set the vertical direction to create a parabolic trajectory
	knockbackDirection.Normalize();

	m_vKnockBackDir = knockbackDirection;
}

void CEnemyScript::SaveToLevelFile(FILE* _File)
{
	//fwrite(&m_iNoticedDoor, sizeof(int), 1, _File);
}

void CEnemyScript::LoadFromLevelFile(FILE* _File)
{
	//fread(&m_iNoticedDoor, sizeof(int), 1, _File);
}
