#include "pch.h"
#include "CPlayerScript.h"
#include <iostream>

#include <Engine\CMeshRender.h>
#include <Engine\CMaterial.h>

#include <Engine\CPrefab.h>
#include <Engine\CEngine.h>
#include <Engine\component.h>
#include <Engine\CFontManager.h>
#include <Engine\CLevel.h>
#include <Engine\CRecordManager.h>
#include <Engine\CAnim2D.h>

#include "CGameManagerScript.h"
#include "CDoorScript.h"

#include "CPlayerIdleState.h"
#include "CPlayerAttackState.h"
#include "CPlayerDoorBreakState.h"
#include "CPlayerFallState.h"
#include "CPlayerFlipState.h"
#include "CPlayerHurtFlyState.h"
#include "CPlayerHurtGroundState.h"
#include "CPlayerIdletoRunState.h"
#include "CPlayerRollState.h"
#include "CPlayerRunState.h"
#include "CPlayerJumpState.h"
#include "CPlayerFallState.h"
#include "CPlayerPreCrouchState.h"
#include "CPlayerCrouchState.h"
#include "CPlayerPostCrouchState.h"
#include "CPlayerRuntoIdleState.h"
#include "CPlayerRollState.h"
#include "CPlayerWallGrabState.h"
#include "CPlayerWallSlideState.h"


#define NOINPUT m_iHorizontalInput == 0.f && m_iVerticalInput == 0.f

CPlayerScript::CPlayerScript()
	: CScript((UINT)SCRIPT_TYPE::PLAYERSCRIPT)
	, m_iHp(1)
	, m_bDead(false)
	, m_fSpeed(30.f)
	, m_fJumpForce(380.f)
	, m_PlatformInfo{}
	, m_arrWallCollision{ 1, }
	, m_fAttackCoolDown(0.4f)
	, m_fKnockBackForce(200.f)
	, m_fForceDuration(0.1f) // Adjust the duration as needed
		, m_fForceCounter(0.0f)
		, m_vForce (0.0f, 0.0f, 0.0f)
		, m_bApplyForce(false)
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fSpeed, "Speed");
	AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fJumpForce, "JumpHeight");
	AddScriptParam(SCRIPT_PARAM::FLOAT, &m_fFallingGravity, "FallingGravity");
	AddScriptParam(SCRIPT_PARAM::INT, &m_bIsRoll, "IsRoll");
	AddScriptParam(SCRIPT_PARAM::INT, &m_bGround, "IsOnGround");
	AddScriptParam(SCRIPT_PARAM::INT, &m_bOnSlope, "IsOnSlope");
}

CPlayerScript::~CPlayerScript()
{

}

void CPlayerScript::begin()
{
	// 동적 재질을 생성하고 현재 재질로 세팅
	MeshRender()->GetDynamicMaterial();

	if (GetOwner()->FSM() == nullptr)
		return;

	// FSM에 State 추가
	GetOwner()->FSM()->AddState(L"PlayerIdle", new CPlayerIdleState);
	GetOwner()->FSM()->AddState(L"PlayerIdleToRun", new CPlayerIdletoRunState);
	GetOwner()->FSM()->AddState(L"PlayerRun", new CPlayerRunState);
	GetOwner()->FSM()->AddState(L"PlayerRunToIdle", new CPlayerRuntoIdleState);
	GetOwner()->FSM()->AddState(L"PlayerJump", new CPlayerJumpState);
	GetOwner()->FSM()->AddState(L"PlayerFall", new CPlayerFallState);
	GetOwner()->FSM()->AddState(L"PlayerPreCrouch", new CPlayerPreCrouchState);
	GetOwner()->FSM()->AddState(L"PlayerCrouch", new CPlayerCrouchState);
	GetOwner()->FSM()->AddState(L"PlayerPostCrouch", new CPlayerPostCrouchState);
	GetOwner()->FSM()->AddState(L"PlayerRoll", new CPlayerRollState);
	GetOwner()->FSM()->AddState(L"PlayerWallGrab", new CPlayerWallGrabState);
	GetOwner()->FSM()->AddState(L"PlayerWallSlide", new CPlayerWallSlideState);
	GetOwner()->FSM()->AddState(L"PlayerFlip", new CPlayerFlipState);
	GetOwner()->FSM()->AddState(L"PlayerAttack", new CPlayerAttackState);
	GetOwner()->FSM()->AddState(L"PlayerDoorBreak", new CPlayerDoorBreakState);
	GetOwner()->FSM()->AddState(L"PlayerHurtFly", new CPlayerHurtFlyState);
	GetOwner()->FSM()->AddState(L"PlayerHurtGround", new CPlayerHurtGroundState);

	// Idle State로 지정
	GetOwner()->FSM()->ChangeState(L"PlayerIdle");

	// 초기 자리
	GetOwner()->Transform()->SetRelativePos(Vec3(-300.f, -200.f, 0.f));

	// 모든 State Reset
	ResetState();
}

void CPlayerScript::tick()
{
	if ((UINT)PLAYMODE::NONE != CRecordManager::GetInst()->GetPlayMode())
		return;

	if (m_iHp <= 0 && !m_bDead)
		GetOwner()->FSM()->ChangeState(L"PlayerHurtFly");

	CheckState();
	CheckInput();
	CheckOnAirTime();

	// 캐릭터 방향 Check
	if (m_iLookDir > 0)
		GetOwner()->Transform()->SetRelativeRot(0.f, 0.f, 0.f);
	else
		GetOwner()->Transform()->SetRelativeRot(0.f, XM_PI, 0.f);

	if (m_PlatformInfo.Type == PLATFORM_TYPE::RAMP && NOINPUT)
	{
		Vec3 newPosition = GetPositionOnRamp();
		GetOwner()->Transform()->SetRelativePos(newPosition);
	}

	CheckWall();
	PlayerMove();
}

void CPlayerScript::BeginOverlap(CCollider2D* _Other)
{
	if (L"ToNextLevel" == _Other->GetOwner()->GetName())
	{
		CLevel* CurLevel = CLevelManager::GetInst()->GetCurLevel();
		CGameObject* GM = CurLevel->FindObjectByName(L"GameManager");
		CGameManagerScript* GMScript = dynamic_cast<CGameManagerScript*>(GM->GetScripts()[0]);
		GMScript->SetLevelClear(true);
	}
}

void CPlayerScript::OnOverlap(CCollider2D* _Other)
{
	if (_Other->GetOwner()->GetScripts().empty())
		return;

	if ((UINT)SCRIPT_TYPE::DOORSCRIPT == _Other->GetOwner()->GetScripts()[0]->GetScriptType())
	{
		// Door과 충돌 중일 경우, 닫힌 Door일 경우
		if ((UINT)DOOR_STATE::CLOSE == _Other->GetOwner()->GetScript<CDoorScript>()->GetDoorState())
		{
			if (KEY_TAP(KEY::SPACE))
			{
				// Door 상태 Open으로 전환
				_Other->GetOwner()->GetScript<CDoorScript>()->SetDoorState((UINT)DOOR_STATE::OPEN);

				// Space 누르면 DoorBreak 상태로 전환
				GetOwner()->FSM()->ChangeState(L"PlayerDoorBreak");
			}
		}
	}
}

void CPlayerScript::EndOverlap(CCollider2D* _Other)
{
}

void CPlayerScript::CheckState()
{
	m_bGround = GetOwner()->RigidBody2D()->IsGround();
	m_bOnSlope = GetOwner()->RigidBody2D()->IsOnSlope();

	if (m_bPlatformBreakDown)
	{
		GetOwner()->RigidBody2D()->SetGround(false);
		GetOwner()->FSM()->ChangeState(L"PlayerFall");
	}

	if (!m_bIsAttackable)
	{
		m_fAttackTimer += DT;
		if (m_fAttackTimer >= m_fAttackCoolDown)
		{
			m_bIsAttackable = true;
			m_fAttackTimer = 0.f;
		}
	}

	if (m_bGround || m_bOnSlope)
	{
		GetOwner()->RigidBody2D()->SetGravity(false);
		m_bPlatformBreakDown = false;
	}
	else
	{
		GetOwner()->RigidBody2D()->SetGravity(true);
	}

	if (m_bIsWallGrab)
	{
		//float fOriginalGravity = GetOwner()->RigidBody2D()->GetGravityAccel();
		GetOwner()->RigidBody2D()->SetGravityAccel(300.f);
		GetOwner()->RigidBody2D()->SetVelocityLimitX(1000.f);
	}
	else
	{
		GetOwner()->RigidBody2D()->SetGravityAccel(1000.f);
		GetOwner()->RigidBody2D()->SetVelocityLimitX(400.f);
	}
	if (m_bApplyForce)
	{
		GetOwner()->RigidBody2D()->AddVelocity(m_vForce * DT);
		m_fForceCounter += DT;

		if (m_fForceCounter >= m_fForceDuration)
		{
			m_bApplyForce = false;
		}
	}

	if (m_bIsRoll)
	{

	}

}

void CPlayerScript::CheckInput()
{
	if (m_iHp <= 0)
		return;

	m_iHorizontalInput = 0;
	m_iVerticalInput = 0;

	if (KEY_TAP(KEY::LBTN) && m_bIsAttackable)
	{
		m_bInput = true;
		
		GetOwner()->RigidBody2D()->SetMovingInput(true);
		GetOwner()->RigidBody2D()->SetGround(false);
		GetOwner()->RigidBody2D()->SetOnSlope(false);

		float fAttackPower = 600.f;
		Vec2 AttackDir = CalcClickDir() * fAttackPower;

		m_vForce = Vec3(AttackDir.x, AttackDir.y, 0.f) * (1.0f / m_fForceDuration);
		
		if (AttackDir.x > 0)
			m_iLookDir = 1;
		else if (AttackDir.x < 0)
			m_iLookDir = -1;

		m_bApplyForce = true;
		m_fForceCounter = 0.0f;

		
		GetOwner()->FSM()->ChangeState(L"PlayerAttack");
		m_bIsAttackable = false;
		return;
	
	}
	if (KEY_PRESSED(KEY::W) && !m_bIsWallGrab)
	{
		m_iVerticalInput += 1;
	}

	if (KEY_PRESSED(KEY::A) && !m_bIsWallGrab)
	{
		m_iLookDir = -1;
		if (m_arrWallCollision[(UINT)WALL_TYPE::BLOCKWALL_LEFT]
			|| m_arrWallCollision[(UINT)WALL_TYPE::HANGABLEWALL_LEFT])
		{
			return;
		}
		else
		{
			m_iHorizontalInput -= 1;
		}
	}

	if (KEY_PRESSED(KEY::S) )
	{
		//m_iVerticalInput -= 1;
	}

	if (KEY_PRESSED(KEY::D) && !m_bIsWallGrab)
	{
		m_iLookDir = 1;
		if (m_arrWallCollision[(UINT)WALL_TYPE::BLOCKWALL_RIGHT]
			|| m_arrWallCollision[(UINT)WALL_TYPE::HANGABLEWALL_RIGHT])
		{
			return;
		}
		else
		{
			m_iHorizontalInput += 1;
		}
	}

	if(NOINPUT || m_bDead)
	{
		m_bInput = false;
		GetOwner()->RigidBody2D()->SetMovingInput(false);
	}
	else
	{
		m_bInput = true;
		GetOwner()->RigidBody2D()->SetMovingInput(true);
		CreateWhiteShadow();
	}
}

void CPlayerScript::CheckOnAirTime()
{
	if (!m_bGround && !m_bOnSlope && !m_bIsWallGrab)
	{
		m_fOnAirTime += DT;
		GetOwner()->RigidBody2D()->SetGravityScale(1 + m_fOnAirTime * 0.7);
	}
	else
	{
		m_fOnAirTime = 0;
		GetOwner()->RigidBody2D()->SetGravityScale(1);
	}	

}

void CPlayerScript::PlayerMove()
{
	if (m_bGround || m_bOnSlope)
	{
		Vec3 Velocity = Vec3(m_iHorizontalInput * m_fSpeed, m_iVerticalInput * m_fJumpForce, 0);

		if (m_PlatformInfo.Type == PLATFORM_TYPE::RAMP)
		{
			GetOwner()->RigidBody2D()->SetGravity(false);

			Vec3 StartPos = m_PlatformInfo.vStartPos;
			Vec3 EndPos = m_PlatformInfo.vEndPos;

			Vec3 playerPos = GetOwner()->Transform()->GetRelativePos();
			bool beyondEndPos = (EndPos.x > StartPos.x && playerPos.x >= EndPos.x) || (EndPos.x < StartPos.x&& playerPos.x <= EndPos.x);

			if (beyondEndPos)
			{
				// If the player is beyond the EndPos, enable gravity and skip the ramp logic
				GetOwner()->RigidBody2D()->SetGravity(true);
			}

			Vec3 StartToEndDir = EndPos - StartPos;
			Vec3 NormalizedDir = StartToEndDir.Normalize();

			Vec3 RampNormal(-NormalizedDir.y, NormalizedDir.x, 0);

			float movementSpeed = m_iHorizontalInput * m_fSpeed * DT;
			Vec3 newPosition = GetOwner()->Transform()->GetRelativePos() + (NormalizedDir * movementSpeed);

			// Calculate the Y position based on the X position of the player
			float t = (newPosition.x - StartPos.x) / (EndPos.x - StartPos.x);
			newPosition.y = StartPos.y + t * (EndPos.y - StartPos.y);

			// Add player's collider half height to the new Y position
			Vec3 playerColliderExtents = GetOwner()->Collider2D()->GetOffsetScale() / 2.f;
			newPosition.y += playerColliderExtents.y;

			// Set the new position directly
			GetOwner()->Transform()->SetRelativePos(newPosition);

			if (NOINPUT)
			{
				GetOwner()->RigidBody2D()->SetVelocity(Vec3(0.f, 0.f, 0.f));
				return;
			}

			if (m_iVerticalInput)
			{
				Velocity.y += m_iVerticalInput * m_fJumpForce;
			}
		}

		else if (m_PlatformInfo.Type == PLATFORM_TYPE::ONEWAY_PLATFORM)
		{
			if (GetOwner()->FSM()->GetCurState()->GetName() == L"PlayerCrouch"
				|| GetOwner()->FSM()->GetCurState()->GetName() == L"PlayerPreCrouch"
				|| GetOwner()->FSM()->GetCurState()->GetName() == L"PlyaerPostCrouch")
			{
				if (KEY_TAP(KEY::S))
					m_bPlatformBreakDown = true;
			}
		}
			
		GetOwner()->RigidBody2D()->AddVelocity(Velocity);
	}
	else
	{
	}

}

void CPlayerScript::CheckWall()
{
	// 공중에서 플레이어 움직임 제어
		if (m_arrWallCollision[(UINT)WALL_TYPE::BLOCKWALL_LEFT] && GetOwner()->RigidBody2D()->GetVelocity().x < 0)
		{
			GetOwner()->RigidBody2D()->SetVelocityX(0.1f);
		}

		if (m_arrWallCollision[(UINT)WALL_TYPE::BLOCKWALL_RIGHT] && GetOwner()->RigidBody2D()->GetVelocity().x > 0)
		{
			GetOwner()->RigidBody2D()->SetVelocityX(0.1f);
		}

		if (m_arrWallCollision[(UINT)WALL_TYPE::BLOCKWALL_TOP] && GetOwner()->RigidBody2D()->GetVelocity().y > 0)
		{
			GetOwner()->RigidBody2D()->SetVelocityY(0);
		}

		if (!m_bGround && !m_bOnSlope && !m_bIsFlip)
		{
			if (m_arrWallCollision[(UINT)WALL_TYPE::HANGABLEWALL_LEFT] && !m_bIsAttack && GetOwner()->RigidBody2D()->GetVelocity().x < 0  && m_iVerticalInput >=1)
			{
				if (m_iHorizontalInput > 0)
				{
					// 공중에서 HangableWall과 부딪쳤지만 반대방향키를 눌렀으면
					GetOwner()->RigidBody2D()->SetVelocityX(0);
				}
				else
				{
					// 공중에서 HangableWall과 부딪침 + 해당방향 방향키 || Flip 상태
					GetOwner()->FSM()->ChangeState(L"PlayerWallGrab");
				}
			}
			else if (m_arrWallCollision[(UINT)WALL_TYPE::HANGABLEWALL_RIGHT] && !m_bIsAttack && GetOwner()->RigidBody2D()->GetVelocity().x > 0 && m_iVerticalInput >=1)
			{
				if (m_iHorizontalInput < 0)
				{
					// 공중에서 HangableWall과 부딪쳤지만 반대방향키를 눌렀으면
					GetOwner()->RigidBody2D()->SetVelocityX(0);
				}
				else
				{
					// 공중에서 HangableWall과 부딪침 + 해당방향 방향키 || Flip 상태
						GetOwner()->FSM()->ChangeState(L"PlayerWallGrab");
				}
			}
		}

}

Vec3 CPlayerScript::GetPositionOnRamp()
{
	if (m_PlatformInfo.Type != PLATFORM_TYPE::RAMP)
	{
		return GetOwner()->Transform()->GetRelativePos();
	}

	Vec3 StartPos = m_PlatformInfo.vStartPos;
	Vec3 EndPos = m_PlatformInfo.vEndPos;

	float t = (GetOwner()->Transform()->GetRelativePos().x - StartPos.x) / (EndPos.x - StartPos.x);
	float newY = StartPos.y + t * (EndPos.y - StartPos.y);

	Vec3 newPosition = GetOwner()->Transform()->GetRelativePos();
	newPosition.y = newY;

	Vec3 playerColliderExtents = GetOwner()->Collider2D()->GetOffsetScale() / 2.f;
	newPosition.y += playerColliderExtents.y;

	return newPosition;
}

//void CPlayerScript::Shoot()
//{
//	// 미사일 프리팹 참조
//	Ptr<CPrefab> pMissilePrefab = CResourceManager::GetInst()->FindRes<CPrefab>(L"MissilePrefab");
//	Vec3 vMissilePos = Transform()->GetRelativePos() + Vec3(0.f, 0.5f, 0.f) * Transform()->GetRelativeScale();
//	CGameObject* pCloneMissile = pMissilePrefab->Instantiate();
//
//	// 레벨에 추가
//	SpawnGameObject(pCloneMissile, vMissilePos, L"PlayerProjectile");
//}

Vec2 CPlayerScript::CalcClickDir()
{
	// 마우스로 클릭 시 플레이어와 마우스 클릭 지점 사이의 정규화된 방향 벡터(길이 1) 계산

	// ===== 1. 플레이어 - 클릭 지점의 거리 계산 ======

	Vec3 PlayerPos = Transform()->GetRelativePos();
	// 마우스 클릭 지점
	Vec2 MouseClickPos = CKeyManager::GetInst()->GetProjMousePos();

	// 피타고라스의 정리로 두 점 사이의 거리를 구한다
	Vec2 vDirection = Vec2(MouseClickPos.x - PlayerPos.x, MouseClickPos.y - PlayerPos.y);
	float fLength  = sqrt(vDirection.x * vDirection.x + vDirection.y * vDirection.y);

	// ==== 2. 구해진 거리를 바탕으로 방향만 남도록 Normalize 한다 =====
	vDirection /= fLength;

	return vDirection;

}

void CPlayerScript::ResetState()
{
	m_iHp = 1.f;
	m_fSpeed = 30.f;
	m_fJumpForce = 380.f;
	m_fOnAirTime = 0.f;
	m_fAttackTimer = 0.f;
	m_fAttackCoolDown = 0.4f;
	m_iLookDir = 1.f;     // 캐릭터 방향( 1: 오른쪽, -1: 왼쪽)

	m_bDead = false;
	m_bIsJump = false;
	m_bIsFall = false;
	m_bIsRoll = false;
	m_bIsWallGrab = false;
	m_bIsFlip = false;
	m_bIsAttack = false;
	m_fKnockBackForce = 200.f;
	m_vAttackDir = Vec2(0.f, 0.f);

	m_PlatformInfo = {};
	int m_arrWallCollision[(UINT)WALL_TYPE::END] = {1,0,0,0,0,0};
}

void CPlayerScript::CalcKnockBackDir(CCollider2D* _Other)
{
	Vec3 playerPos = GetOwner()->Transform()->GetRelativePos();
	Vec3 ProjectilePos = _Other->GetOwner()->Collider2D()->GetColliderWorldPos();
	Vec3 knockbackDirection = playerPos - ProjectilePos;
	knockbackDirection.y = 1.0f; // Set the vertical direction to create a parabolic trajectory
	knockbackDirection.Normalize();

	m_vKnockBackDir = knockbackDirection;
}

void CPlayerScript::ResetWallCollision()
{
	for (size_t i = 0; i < (UINT)WALL_TYPE::END; i++)
	{
		m_arrWallCollision[i] = 0;
	}

	m_arrWallCollision[(UINT)WALL_TYPE::NONE] = 1;
}

void CPlayerScript::CreateShadow()
{
	//return;
	CGameObject* Shadow = new CGameObject;
	Shadow->SetName(L"Shadow");
	Shadow->AddComponent(new CTransform);
	Shadow->Transform()->SetAbsolute(true);
	Shadow->Transform()->SetRelativeScale(GetOwner()->Transform()->GetRelativeScale());
	Shadow->Transform()->SetRelativePos(GetOwner()->Transform()->GetRelativePos());
	Shadow->Transform()->SetRelativeRot(GetOwner()->Transform()->GetRelativeRot());
	
	int CurFrmIdx = GetOwner()->Animator2D()->GetCurAnim()->GetCurFrameint();
	CAnimator2D* CloneAnimator = GetOwner()->Animator2D()->Clone();
	Shadow->AddComponent(CloneAnimator);
	Shadow->Animator2D()->GetCurAnim()->SetCurFrame(CurFrmIdx);
	Shadow->Animator2D()->Pause();

	Shadow->AddComponent(new CMeshRender);
	Shadow->MeshRender()->SetMesh(CResourceManager::GetInst()->FindRes<CMesh>(L"RectMesh"));
	Shadow->MeshRender()->SetMaterial(CResourceManager::GetInst()->FindRes<CMaterial>(L"material\\ShadowMtrl.mtrl"));
	int OverlayUse = 1;
	int TransParencyUse = 1;
	vector<Vec4> BrightColors =
	{
		Vec4{ 0.0f, 0.5f, 1.0f, 1.0f }, // BrightBlue
		Vec4{ 1.0f, 0.0f, 1.0f, 1.0f }	// BrightPink
	};
	static size_t counter = 0;
	Vec4 ChosenColor = BrightColors[counter];
	counter = (counter + 1) % BrightColors.size();
	float OverlayAlpha = 0.3f;
	Shadow->MeshRender()->GetMaterial()->SetScalarParam(VEC4_0, &ChosenColor);
	Shadow->MeshRender()->GetMaterial()->SetScalarParam(INT_1, &OverlayUse);
	Shadow->MeshRender()->GetMaterial()->SetScalarParam(INT_2, &TransParencyUse);
	Shadow->MeshRender()->GetMaterial()->SetScalarParam(FLOAT_0, &OverlayAlpha);
	
	Shadow->SetLifeSpan(0.1f);
	SpawnGameObject(Shadow, Shadow->Transform()->GetRelativePos(), L"Effect");
}

void CPlayerScript::CreateWhiteShadow()
{
	CGameObject* Shadow = new CGameObject;
	Shadow->SetName(L"Shadow");
	Shadow->AddComponent(new CTransform);
	Shadow->Transform()->SetAbsolute(true);
	Shadow->Transform()->SetRelativeScale(GetOwner()->Transform()->GetRelativeScale());
	Shadow->Transform()->SetRelativePos(GetOwner()->Transform()->GetRelativePos());
	Shadow->Transform()->SetRelativeRot(GetOwner()->Transform()->GetRelativeRot());

	int CurFrmIdx = GetOwner()->Animator2D()->GetCurAnim()->GetCurFrameint();
	CAnimator2D* CloneAnimator = GetOwner()->Animator2D()->Clone();
	Shadow->AddComponent(CloneAnimator);
	Shadow->Animator2D()->GetCurAnim()->SetCurFrame(CurFrmIdx);
	Shadow->Animator2D()->Pause();

	Shadow->AddComponent(new CMeshRender);
	Shadow->MeshRender()->SetMesh(CResourceManager::GetInst()->FindRes<CMesh>(L"RectMesh"));
	Shadow->MeshRender()->SetMaterial(CResourceManager::GetInst()->FindRes<CMaterial>(L"material\\ShadowMtrl.mtrl"));
	int OverlayUse = 1;
	int TransParencyUse = 1;

	Vec4 ChosenColor = Vec4(1.f, 1.f, 1.f, 1.f);

	float OverlayAlpha = 0.2f;
	Shadow->MeshRender()->GetMaterial()->SetScalarParam(VEC4_0, &ChosenColor);
	Shadow->MeshRender()->GetMaterial()->SetScalarParam(INT_1, &OverlayUse);
	Shadow->MeshRender()->GetMaterial()->SetScalarParam(INT_2, &TransParencyUse);
	Shadow->MeshRender()->GetMaterial()->SetScalarParam(FLOAT_0, &OverlayAlpha);

	Shadow->SetLifeSpan(0.05f);
	SpawnGameObject(Shadow, Shadow->Transform()->GetRelativePos(), L"Effect");
}

void CPlayerScript::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_fSpeed, sizeof(float), 1, _File);
}

void CPlayerScript::LoadFromLevelFile(FILE* _File)
{
	fread(&m_fSpeed, sizeof(float), 1, _File);
}
