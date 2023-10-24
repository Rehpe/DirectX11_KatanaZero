#include "pch.h"
#include "CGroundScript.h"
#include "CPlayerScript.h"
#include "CEnemyGruntScript.h"
#include "CEnemyScript.h"

CGroundScript::CGroundScript()
	: CScript((UINT)SCRIPT_TYPE::GROUNDSCRIPT)
{
}

CGroundScript::~CGroundScript()
{
}

void CGroundScript::begin()
{
}

void CGroundScript::tick()
{
}

void CGroundScript::BeginOverlap(CCollider2D* _Other)
{
}

void CGroundScript::OnOverlap(CCollider2D* _Other)
{
	if (L"Player" == _Other->GetOwner()->GetName())
	{	
		CGameObject* Player = _Other->GetOwner();
		CPlayerScript* PlayerScript = Player->GetScript<CPlayerScript>();
		tPlatformColliderInfo preInfo = PlayerScript->GetPlatformInfo();

		tPlatformColliderInfo info = {};

		info.Type = PLATFORM_TYPE::GROUND;
		info.Priority = 5;

		if (preInfo.Priority != 0 && preInfo.Priority < info.Priority)
		{
			return;	// 우선순위 낮을 시 return;
		}
		else
		{
			PlayerScript->SetPlatformInfo(info);

			Player->RigidBody2D()->SetGround(true);
			AntiOverlap(_Other);
		}
	}
	else if (L"Monster" == _Other->GetOwner()->GetLayerName())
	{
		CEnemyScript* EnemyScript = _Other->GetOwner()->GetScript<CEnemyScript>();

		if (EnemyScript->IsUsingRamp())
			return;

		EnemyScript->SetGround(true);
		AntiOverlap(_Other);
	}
}

void CGroundScript::EndOverlap(CCollider2D* _Other)
{
	if (L"Player" == _Other->GetOwner()->GetName())
	{
		CGameObject* Player = _Other->GetOwner();
		CPlayerScript* PlayerScript = Player->GetScript<CPlayerScript>();

		Player->RigidBody2D()->SetGround(false);

		tPlatformColliderInfo info = {};

		PlayerScript->SetPlatformInfo(info);
	}
	else if (L"Monster" == _Other->GetOwner()->GetLayerName())
	{
		CEnemyScript* EnemyScript = _Other->GetOwner()->GetScript<CEnemyScript>();
		EnemyScript->SetGround(false);
	}
}

void CGroundScript::AntiOverlap(CCollider2D* _ObjCol)
{
	// 충돌체 파묻히는 것 꺼내기
	Vec3 overlap = CalculateOverlapAmount(_ObjCol, GetOwner()->Collider2D());
	float threshhold = 1.f;

	Vec3 ObjectPos = _ObjCol->GetOwner()->Transform()->GetRelativePos();
	if (overlap.y >= threshhold)
	{
		ObjectPos.y += overlap.y;
		_ObjCol->GetOwner()->Transform()->SetRelativePos(ObjectPos);

		_ObjCol->GetOwner()->RigidBody2D()->SetVelocityY(0);
	}
}

//Vec3 CGroundScript::CalculateOverlapAmount(CCollider2D* _colliderA, CCollider2D* _colliderB)
//{
//	Vec3 posA = _colliderA->GetOwner()->Transform()->GetRelativePos();
//	Vec3 posB = _colliderB->GetOwner()->Transform()->GetRelativePos();
//	Vec3 extentsA = _colliderA->GetOffsetScale() / 2.f;
//	Vec3 extentsB = _colliderB->GetOffsetScale() / 2.f;
//
//	// Calculate the overlapping amounts for each axis
//	float xOverlap = (extentsA.x + extentsB.x) - abs(posA.x - posB.x);
//	float yOverlap = (extentsA.y + extentsB.y) - abs(posA.y - posB.y);
//
//	// Return the overlap amounts
//	return Vec3(xOverlap, yOverlap, 0);
//}
