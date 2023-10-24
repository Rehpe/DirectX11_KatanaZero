#include "pch.h"
#include "COneWayPlatformScript.h"
#include "CPlayerScript.h"
#include "CEnemyScript.h"

COneWayPlatformScript::COneWayPlatformScript()
	:CScript((UINT)SCRIPT_TYPE::ONEWAYPLATFORMSCRIPT)
{
}

COneWayPlatformScript::~COneWayPlatformScript()
{
}

void COneWayPlatformScript::tick()
{
}

void COneWayPlatformScript::BeginOverlap(CCollider2D* _Other)
{
}

void COneWayPlatformScript::OnOverlap(CCollider2D* _Other)
{
	if (L"Player" == _Other->GetOwner()->GetName())
	{
		//플레이어의 발밑이 해당 충돌체의 표면 위에 있는지 계산
		CGameObject* Player = _Other->GetOwner();
		CPlayerScript* PlayerScript = Player->GetScript<CPlayerScript>();
		
		Vec3 PlayerColliderExtent = Player->Collider2D()->GetOffsetScale() / 2.f;
		Vec3 PlayerFeetPos = Player->Transform()->GetRelativePos() -
							Vec3(0, PlayerColliderExtent.y, 0);
		Vec3 PlatformColliderExtent = GetOwner()->Collider2D()->GetOffsetScale() / 2.f;
		Vec3 PlatformSurface = GetOwner()->Transform()->GetRelativePos() +
								Vec3(0, PlatformColliderExtent.y, 0);

		bool IsPlayerAbove = PlayerFeetPos.y >= PlatformSurface.y - 30.f;

		float PlayerVelocityY = Player->RigidBody2D()->GetVelocity().y;

		if (!IsPlayerAbove || PlayerVelocityY > 0)
			return;
		
		// 플레이어의 발 밑이 해당 충돌체 표면보다 위에 있다면, 플레이어에게 플랫폼 정보 제공
		tPlatformColliderInfo preInfo = PlayerScript->GetPlatformInfo();

		tPlatformColliderInfo info = {};

		info.Type = PLATFORM_TYPE::ONEWAY_PLATFORM;
		info.Priority = 2;

		if (preInfo.Priority != 0 && preInfo.Priority < info.Priority)
		{
			return;	// 우선순위 낮을 시 return;
		}
		else
		{
			PlayerScript->SetPlatformInfo(info);

			if (PlayerScript->IsPlatformBreakdown())
			{
				Player->RigidBody2D()->SetGround(false);
			}
			else
			{
				Player->RigidBody2D()->SetGround(true);

				// 충돌체 파묻히는 것 꺼내기
				Vec3 overlap = CalculateOverlapAmount(_Other, GetOwner()->Collider2D());
				float threshhold = 1.f;

				// Push the player out of the ground
				Vec3 playerPos = Player->Transform()->GetRelativePos();
				if (overlap.y >= threshhold)
				{
					playerPos.y += overlap.y;
					Player->Transform()->SetRelativePos(playerPos);

					Player->RigidBody2D()->SetVelocityY(0);
				}
			}
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

void COneWayPlatformScript::EndOverlap(CCollider2D* _Other)
{
	if (L"Player" == _Other->GetOwner()->GetName())
	{
		CGameObject* Player = _Other->GetOwner();
		CPlayerScript* PlayerScript = Player->GetScript<CPlayerScript>();

		Player->RigidBody2D()->SetGround(false);

		tPlatformColliderInfo info = {};

		PlayerScript->SetPlatformInfo(info);
	}
}

//Vec3 COneWayPlatformScript::CalculateOverlapAmount(CCollider2D* _colliderA, CCollider2D* _colliderB)
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

void COneWayPlatformScript::AntiOverlap(CCollider2D* _ObjCol)
{
	// 충돌체 파묻히는 것 꺼내기
	Vec3 overlap = CalculateOverlapAmount(_ObjCol, GetOwner()->Collider2D());
	float threshhold = 1.f;

	// Push the player out of the ground
	Vec3 ObjectPos = _ObjCol->GetOwner()->Transform()->GetRelativePos();
	if (overlap.y >= threshhold)
	{
		ObjectPos.y += overlap.y;
		_ObjCol->GetOwner()->Transform()->SetRelativePos(ObjectPos);

		_ObjCol->GetOwner()->RigidBody2D()->SetVelocityY(0);
	}
}
