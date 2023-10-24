#include "pch.h"
#include "CPlayerAttackState.h"

#include <Engine\CFSM.h>
#include <Engine\CAnimator2D.h>
#include <Engine\CAnim2D.h>

#include <Engine\CKeyManager.h>
#include <Engine\CRigidBody2D.h>
#include <Engine\CResourceManager.h>
#include <Engine\CLevel.h>

#include "CScriptManager.h"
#include "CPlayerScript.h"


#include <cmath>

CPlayerAttackState::CPlayerAttackState()
	: m_fAttackPower(500.f)
{
}

CPlayerAttackState::~CPlayerAttackState()
{
}

void CPlayerAttackState::finaltick()
{
	CGameObject* pPlayer = GetOwner();
	CPlayerScript* pPlayerScript = pPlayer->GetScript<CPlayerScript>();

	pPlayerScript->SetAttack(true);
	pPlayerScript->CreateShadow();

	if (GetOwner()->Animator2D()->GetCurAnim()->IsFinish()
		|| GetOwner()->RigidBody2D()->GetVelocity().y < 0)
	{
		// Attack 애니메이션이 끝났거나 떨어지면 Fall
		pPlayerScript->SetAttack(false);
		GetOwnerFSM()->ChangeState(L"PlayerFall");
		return;
	}
}

void CPlayerAttackState::Enter()
{
	// Attack Anim 재생
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"PlayerAttack", false);

	// Attack Sprite 생성
	Ptr<CPrefab> pPrefab = CResourceManager::GetInst()->FindRes<CPrefab>(L"prefab\\AttackSprite.pref");
	CGameObject* pAttackSprite = pPrefab->Instantiate();

	CGameObject* pPlayer = GetOwner();
	CPlayerScript* pPlayerScript = pPlayer->GetScript<CPlayerScript>();
	
	// Attack Direction 계산
	m_vecAttackDir = CalcClickDir();
	pPlayerScript->SetAttackDir(m_vecAttackDir);

	pAttackSprite->Animator2D()->FindAnim(L"AttackSprite")->Reset();

	// 현재 레벨에 Sprite 생성
	Vec3 pPlayerPos = GetOwner()->Transform()->GetRelativePos();
	SpawnGameObject(pAttackSprite, pPlayerPos, L"PlayerProjectile");

	// Attack Sprite 회전
	float fAngle = atan2(m_vecAttackDir.y, m_vecAttackDir.x);
	pAttackSprite->Transform()->SetRelativeRot(Vec3(0.f, 0.f, fAngle));

	// Sound Effect
	Ptr<CSound> AttackSFX = CResourceManager::GetInst()->FindRes<CSound>(L"sound\\SFX\\Player_Slash1.wav");
	AttackSFX->Play(1, CTimeManager::GetInst()->GetTimeFactor(), 0.5f, false);

}

void CPlayerAttackState::Exit()
{
	// Attack Anim Reset
	GetOwnerFSM()->GetOwner()->Animator2D()->FindAnim(L"PlayerAttack")->Reset();
}


Vec2 CPlayerAttackState::CalcClickDir()
{
	// 마우스로 클릭 시 플레이어와 마우스 클릭 지점 사이의 정규화된 방향 벡터(길이 1) 계산

	// ===== 1. 플레이어 - 클릭 지점의 거리 계산 ======

	Vec3 PlayerPos = GetOwner()->Transform()->GetRelativePos();
	// 마우스 클릭 지점
	Vec2 MouseClickPos = CKeyManager::GetInst()->GetProjMousePos();

	// 피타고라스의 정리로 두 점 사이의 거리를 구한다
	Vec2 vDirection = Vec2(MouseClickPos.x - PlayerPos.x, MouseClickPos.y - PlayerPos.y);
	float fLength = sqrt(vDirection.x * vDirection.x + vDirection.y * vDirection.y);

	// ==== 2. 구해진 거리를 바탕으로 방향만 남도록 Normalize 한다 =====
	vDirection /= fLength;

	return vDirection;

}