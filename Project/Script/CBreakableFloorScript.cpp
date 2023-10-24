#include "pch.h"
#include "CBreakableFloorScript.h"
#include "CPlayerScript.h"

CBreakableFloorScript::CBreakableFloorScript()
	:CScript((UINT)SCRIPT_TYPE::BREAKABLEFLOORSCRIPT)
{
}

CBreakableFloorScript::~CBreakableFloorScript()
{
}

void CBreakableFloorScript::tick()
{
}

void CBreakableFloorScript::BeginOverlap(CCollider2D* _Other)
{
}

void CBreakableFloorScript::OnOverlap(CCollider2D* _Other)
{
	if (L"Player" == _Other->GetOwner()->GetName())
	{
		CGameObject* Player = _Other->GetOwner();
		CPlayerScript* PlayerScript = Player->GetScript<CPlayerScript>();
		tPlatformColliderInfo preInfo = PlayerScript->GetPlatformInfo();

		tPlatformColliderInfo info = {};

		info.Type = PLATFORM_TYPE::BREAKABLE_FLOOR;
		info.Priority = 5;

		if (preInfo.Priority != 0 && preInfo.Priority < info.Priority)
		{
			return;	// 우선순위 낮을 시 return;
		}
		else
		{
			PlayerScript->SetPlatformInfo(info);

			CollisionDir collisionDir = GetCollisionDirY(_Other, GetOwner()->Collider2D());
			
			if (collisionDir == CollisionDir::DOWN)	// 플레이어가 지형 아래에서 머리 박음
			{
				// 충돌체 파묻히는 것 꺼내기
				Vec3 overlap = CalculateOverlapAmount(_Other, GetOwner()->Collider2D());
				float threshhold = 1.f;

				// Push the player out of the ground
				Vec3 playerPos = Player->Transform()->GetRelativePos();
				if (overlap.y >= threshhold)
				{
					playerPos.y -= overlap.y;
					Player->Transform()->SetRelativePos(playerPos);
				}
			}

			else if (collisionDir == CollisionDir::UP)	// 플레이어가 지형 위에 서있음
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

	if (L"AttackSprite" == _Other->GetOwner()->GetName())
	{
		GetOwner()->Collider2D()->SetOffsetScale(Vec2(0.f, 0.f));
	}
}

void CBreakableFloorScript::EndOverlap(CCollider2D* _Other)
{
}

//CollisionDir CBreakableFloorScript::GetCollisionDir(CCollider2D* _colliderA, CCollider2D* _colliderB)
//{
//	Vec3 posA = _colliderA->GetOwner()->Transform()->GetRelativePos();
//	Vec3 posB = _colliderB->GetOwner()->Transform()->GetRelativePos();
//	Vec3 deltaPos = posB - posA;
//
//	float deltaY = abs(deltaPos.y);
//
//	if (deltaPos.y > 0)
//	{
//		return CollisionDir::UP;
//	}
//	else
//	{
//		return CollisionDir::DOWN;
//	}
//}

//Vec3 CBreakableFloorScript::CalculateOverlapAmount(CCollider2D* _colliderA, CCollider2D* _colliderB)
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