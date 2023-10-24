#include "pch.h"
#include "CGangsterRunState.h"

#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>
#include <Engine\CLevelManager.h>
#include <Engine\CLevel.h>
#include <Engine\CTransform.h>
#include <Engine\CRigidBody2D.h>

#include "CEnemyGruntScript.h"
#include "CEnemyGangsterScript.h"


CGangsterRunState::CGangsterRunState()
{
	SetName(L"Run");
}

CGangsterRunState::~CGangsterRunState()
{
}

void CGangsterRunState::finaltick()
{
	CGameObject* pEnemy = GetOwner();
	CEnemyScript* EnemyScript = dynamic_cast<CEnemyScript*>(GetOwner()->GetScripts()[0]);
	CEnemyGangsterScript* GangsterScript = dynamic_cast<CEnemyGangsterScript*>(GetOwner()->GetScripts()[0]);

	float m_fRunSpeed = 35.f;

	// 속도 제한
	pEnemy->RigidBody2D()->SetVelocityLimitX(200.f);

	if (EnemyScript->IsCollidingWithPlayer())
	{
		// 플레이어 충돌체와 몸통이 충돌 중이라면 Attack
		GetOwnerFSM()->ChangeState(L"Attack");
	}
	else if (EnemyScript->IsDetectingPlayer() && !EnemyScript->IsCollidingWithPlayer()
		&& GangsterScript->IsShootable() && !EnemyScript->IsDetectingDoor())
	{
		// Detector 충돌 중 + Shootable이라면 Aim
		GetOwnerFSM()->ChangeState(L"Aim");
	}

	if (EnemyScript->IsUsingRamp())
	{
		Vec3 StartPos = EnemyScript->GetCollidingRampStartPos();
		Vec3 EndPos = EnemyScript->GetCollidingRampEndPos();

		if (StartPos == EndPos)
			return;

		MoveAlongRamp(StartPos, EndPos);
	}

	Vec3 EnemyPos = GetOwner()->Transform()->GetRelativePos();

	if (EnemyScript->IsPlayerOnSameFloor())
	{
		// 플레이어의 위치로 달려간다
		CLevel* pLevel = CLevelManager::GetInst()->GetCurLevel();
		CGameObject* pPlayer = pLevel->FindObjectByName(L"Player");

		Vec3 PlayerPos = pPlayer->Transform()->GetRelativePos();

		// 피타고라스의 정리로 두 점 사이의 거리를 구한다
		Vec2 vDirection = Vec2(PlayerPos.x - EnemyPos.x, PlayerPos.y - EnemyPos.y);
		float fLength = sqrt(vDirection.x * vDirection.x + vDirection.y * vDirection.y);

		// ==== 2. 구해진 거리를 바탕으로 방향만 남도록 Normalize 한다 =====
		vDirection /= fLength;

		if (vDirection.x > 0)
		{
			EnemyScript->SetLookDir(1);
		}
		else if (vDirection.x < 0)
		{
			EnemyScript->SetLookDir(-1);
		}

		// vDirection 방향으로 AddVelocity한다
		GetOwner()->RigidBody2D()->AddVelocity(Vec3(vDirection.x * m_fRunSpeed, 0.f, 0.f));
	}
	else if (!EnemyScript->IsPlayerOnSameFloor() && !EnemyScript->IsUsingRamp())
	{
		// 가까운 경사로로 달려간다
		Vec3 NearestRampPos = EnemyScript->FindNearestRampPos();

		// 피타고라스의 정리로 두 점 사이의 거리를 구한다
		Vec2 vDirection = Vec2(NearestRampPos.x - EnemyPos.x, NearestRampPos.y - EnemyPos.y);
		float fLength = sqrt(vDirection.x * vDirection.x + vDirection.y * vDirection.y);

		// ==== 2. 구해진 거리를 바탕으로 방향만 남도록 Normalize 한다 =====
		vDirection /= fLength;

		if (vDirection.x > 0)
		{
			EnemyScript->SetLookDir(1);
		}
		else if (vDirection.x < 0)
		{
			EnemyScript->SetLookDir(-1);
		}

		// vDirection 방향으로 AddVelocity한다
		GetOwner()->RigidBody2D()->AddVelocity(Vec3(vDirection.x * m_fRunSpeed, 0.f, 0.f));
	}
}

void CGangsterRunState::Enter()
{
	// GangsterTurn Anim 재생
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"GangsterWalk", true);

	// 인식 상태 On
	CGameObject* pEnemy = GetOwner();
	CEnemyScript* EnemyScript = dynamic_cast<CEnemyScript*>(GetOwner()->GetScripts()[0]);

	EnemyScript->SetNoticed(true);
}

void CGangsterRunState::Exit()
{
	// GruntRun Anim Reset
	//GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"GruntRun")->Reset();
}

void CGangsterRunState::MoveAlongRamp(const Vec3& _StartPos, const Vec3& _EndPos)
{
	Vec3 EnemyPos = GetOwner()->Transform()->GetRelativePos();
	bool beyondEndPos = (_EndPos.x > _StartPos.x && EnemyPos.x >= _EndPos.x) || (_EndPos.x < _StartPos.x&& EnemyPos.x <= _EndPos.x);

	if (beyondEndPos)
	{
		// If the enemy is beyond the EndPos, enable gravity and skip the ramp logic
		GetOwner()->RigidBody2D()->SetGravity(true);
		return;
	}

	Vec3 StartToEndDir = _EndPos - _StartPos;
	Vec3 NormalizedDir = StartToEndDir.Normalize();

	float m_fRunSpeed = 35.f;
	float movementSpeed = m_fRunSpeed * DT;

	Vec3 newPosition = GetOwner()->Transform()->GetRelativePos() + (NormalizedDir * movementSpeed);

	// Calculate the Y position based on the X position of the enemy
	float t = (newPosition.x - _StartPos.x) / (_EndPos.x - _StartPos.x);
	newPosition.y = _StartPos.y + t * (_EndPos.y - _StartPos.y);

	// Add enemy's collider half height to the new Y position
	Vec3 enemyColliderExtents = GetOwner()->Collider2D()->GetOffsetScale() / 2.f;
	newPosition.y += enemyColliderExtents.y;

	// Set the new position directly
	GetOwner()->Transform()->SetRelativePos(newPosition);
}
