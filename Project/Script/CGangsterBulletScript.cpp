#include "pch.h"
#include "CGangsterBulletScript.h"

#include <Engine\CLevel.h>
#include <Engine\CLayer.h>
#include <Engine\CRecordManager.h>

#include "CPlayerScript.h"
#include "CEnemyScript.h"
#include "CEffectManager.h"
#include "CGameManagerScript.h"
#include "CCameraScript.h"


CGangsterBulletScript::CGangsterBulletScript()
	:CScript((UINT)SCRIPT_TYPE::GANGSTERBULLETSCRIPT)
	, m_fBulletSpeed(500.f)
	, m_pShooter(nullptr)
	, m_bReflect(false)
{
}

CGangsterBulletScript::~CGangsterBulletScript()
{
}

void CGangsterBulletScript::begin()
{
	if ((UINT)PLAYMODE::NONE != CRecordManager::GetInst()->GetPlayMode())
		return;

	GetOwner()->RigidBody2D()->SetGravity(false);
	GetOwner()->RigidBody2D()->SetVelocity(Vec3(0.f, 0.f, 0.f));
	GetOwner()->RigidBody2D()->SetVelocityLimit(Vec3(600.f, 0.f, 0.f));

	// begin �Ǵ� �������� Player ��ġ�� ���� ����
	CLevel* pCurLevel = CLevelManager::GetInst()->GetCurLevel();
	CGameObject* Player = pCurLevel->FindObjectByName(L"Player");
	Vec3 PlayerPos = Player->Transform()->GetRelativePos();

	Vec3 ShootDir = PlayerPos - m_pShooter->Transform()->GetRelativePos();
	ShootDir = ShootDir.Normalize();

	m_vTargetDir = ShootDir;

	// ��ǥ �������� ȸ��
	float fAngle = atan2(m_vTargetDir.y, m_vTargetDir.x);
	Transform()->SetRelativeRot(Vec3(0.f, 0.f, fAngle));

	// �߻� �� 3�� �Ŀ� Destroy
	SetLifeSpan(3.f);
}

void CGangsterBulletScript::tick()
{
	if (!m_bReflect)
		// ��ǥ �������� �߻�
		GetOwner()->RigidBody2D()->AddVelocity(Vec3(m_vTargetDir.x * m_fBulletSpeed, 0.f, 0.f));
	else
		Reflect();
}

void CGangsterBulletScript::BeginOverlap(CCollider2D* _Other)
{
	CGameObject* Player = _Other->GetOwner();
	CPlayerScript* PlayerScript = Player->GetScript<CPlayerScript>();

	// ���� ������ ������ Destroy
	if (L"Ground" == _Other->GetOwner()->GetLayerName()
		|| L"Wall" == _Other->GetOwner()->GetLayerName()
		|| L"Ramp" == _Other->GetOwner()->GetLayerName()
		|| L"Door" == _Other->GetOwner()->GetLayerName())
	{
		Destroy();
		return;
	}

	// MonsterProjectile ������ �� PlayerProjectile �� �浹�ϸ� Reflect
	if (L"MonsterProjectile" == GetOwner()->GetLayerName())
	{
		if (L"PlayerProjectile" == _Other->GetOwner()->GetLayerName())
		{
			// Bullet Layer ����
			CGameObject* Bullet = GetOwner();
			CLevel* pCurLevel = CLevelManager::GetInst()->GetCurLevel();
			int PlayerProjectileIdx = pCurLevel->FindLayerByName(L"PlayerProjectile")->GetLayerIndex();
			Bullet->ChangeLayer(PlayerProjectileIdx);
			Bullet->RigidBody2D()->SetVelocity(Vec3(0.f, 0.f, 0.f));

			CEffectManager::CreateEffect(EffectType::BULLETREFLECT, Bullet->Transform()->GetRelativePos(), true);
			CLevel* pLevel = CLevelManager::GetInst()->GetCurLevel();
			CGameObject* Camera = pLevel->FindObjectByName(L"MainCamera");
			CCameraScript* CamScript = Camera->GetScript<CCameraScript>();
			CamScript->ApplyCameraShake(0.5f, 40.f);

			// Sound Effect
			Ptr<CSound> BulletReflectSFX = CResourceManager::GetInst()->FindRes<CSound>(L"sound\\SFX\\Reflect_Bullet.wav");
			BulletReflectSFX->Play(1, CTimeManager::GetInst()->GetTimeFactor(), 0.5f, false);

			m_bReflect= true;
			return;
		}

		// Player�� �浹�ϸ� Player Hp ����
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
			Destroy();
		}
	}

	// �ݻ�� ���� Enemy�� �浹�ϸ� Enemy Hp ����
	if (L"PlayerProjectile" == GetOwner()->GetLayerName())
	{
		if (L"Monster" == _Other->GetOwner()->GetLayerName())
		{
			CGameObject* Enemy = _Other->GetOwner();
			CEnemyScript* EnemyScript = Enemy->GetScript<CEnemyScript>();

			EnemyScript->CalcKnockBackDir(this->Collider2D());
			EnemyScript->ReduceHp(1);
			Destroy();
		}
	}
}

void CGangsterBulletScript::OnOverlap(CCollider2D* _Other)
{
}

void CGangsterBulletScript::EndOverlap(CCollider2D* _Other)
{
}


void CGangsterBulletScript::CalcReflectDir(CCollider2D* _Attack)
{
	Vec3 BulletPos = GetOwner()->Transform()->GetRelativePos();
	Vec3 AttackPos = _Attack->GetOwner()->Collider2D()->GetColliderWorldPos();
	Vec3 ReflectDirection = AttackPos - BulletPos;
	ReflectDirection.Normalize();

	m_vReflectDir = ReflectDirection;
}

void CGangsterBulletScript::Reflect()
{
	CGameObject* Bullet = GetOwner();
	Bullet->RigidBody2D()->AddVelocity(Vec3(-m_vTargetDir.x * m_fBulletSpeed, 0.f, 0.f));
}