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
		// Attack �ִϸ��̼��� �����ų� �������� Fall
		pPlayerScript->SetAttack(false);
		GetOwnerFSM()->ChangeState(L"PlayerFall");
		return;
	}
}

void CPlayerAttackState::Enter()
{
	// Attack Anim ���
	GetOwnerFSM()->GetOwner()->Animator2D()->Play(L"PlayerAttack", false);

	// Attack Sprite ����
	Ptr<CPrefab> pPrefab = CResourceManager::GetInst()->FindRes<CPrefab>(L"prefab\\AttackSprite.pref");
	CGameObject* pAttackSprite = pPrefab->Instantiate();

	CGameObject* pPlayer = GetOwner();
	CPlayerScript* pPlayerScript = pPlayer->GetScript<CPlayerScript>();
	
	// Attack Direction ���
	m_vecAttackDir = CalcClickDir();
	pPlayerScript->SetAttackDir(m_vecAttackDir);

	pAttackSprite->Animator2D()->FindAnim(L"AttackSprite")->Reset();

	// ���� ������ Sprite ����
	Vec3 pPlayerPos = GetOwner()->Transform()->GetRelativePos();
	SpawnGameObject(pAttackSprite, pPlayerPos, L"PlayerProjectile");

	// Attack Sprite ȸ��
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
	// ���콺�� Ŭ�� �� �÷��̾�� ���콺 Ŭ�� ���� ������ ����ȭ�� ���� ����(���� 1) ���

	// ===== 1. �÷��̾� - Ŭ�� ������ �Ÿ� ��� ======

	Vec3 PlayerPos = GetOwner()->Transform()->GetRelativePos();
	// ���콺 Ŭ�� ����
	Vec2 MouseClickPos = CKeyManager::GetInst()->GetProjMousePos();

	// ��Ÿ����� ������ �� �� ������ �Ÿ��� ���Ѵ�
	Vec2 vDirection = Vec2(MouseClickPos.x - PlayerPos.x, MouseClickPos.y - PlayerPos.y);
	float fLength = sqrt(vDirection.x * vDirection.x + vDirection.y * vDirection.y);

	// ==== 2. ������ �Ÿ��� �������� ���⸸ ������ Normalize �Ѵ� =====
	vDirection /= fLength;

	return vDirection;

}