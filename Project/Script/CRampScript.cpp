#include "pch.h"
#include "CRampScript.h"
#include "CPlayerScript.h"

CRampScript::CRampScript()
	:CScript((UINT)SCRIPT_TYPE::RAMPSCRIPT)
{
}

CRampScript::~CRampScript()
{
}

void CRampScript::tick()
{
}

void CRampScript::BeginOverlap(CCollider2D* _Other)
{
}

void CRampScript::OnOverlap(CCollider2D* _Other)
{
	if (L"Player" == _Other->GetOwner()->GetName())
	{
		CGameObject* Player = _Other->GetOwner();
		CPlayerScript* PlayerScript = Player->GetScript<CPlayerScript>();
		tPlatformColliderInfo preInfo = PlayerScript->GetPlatformInfo();

		tPlatformColliderInfo info = {};

		info.Type = PLATFORM_TYPE::RAMP;
		info.Priority = 1;
		info.vStartPos = GetOwner()->Collider2D()->GetStartPos();
		info.vEndPos = GetOwner()->Collider2D()->GetEndPos();

		if (preInfo.Priority != 0 && preInfo.Priority < info.Priority)
		{
			return;	// 우선순위 낮을 시 return;
		}
		else
		{
			PlayerScript->SetPlatformInfo(info);
			Player->RigidBody2D()->SetOnSlope(true);
		}
	}
}

void CRampScript::EndOverlap(CCollider2D* _Other)
{
	if (L"Player" == _Other->GetOwner()->GetName())
	{
		CGameObject* Player = _Other->GetOwner();
		CPlayerScript* PlayerScript = Player->GetScript<CPlayerScript>();

		Player->RigidBody2D()->SetOnSlope(false);

		tPlatformColliderInfo info = {};

		PlayerScript->SetPlatformInfo(info);
	}
}

void CRampScript::ResolveCollision(CCollider2D* _colliderA, CCollider2D* _colliderB)
{
	// Get the center positions and extents of the player and ground colliders
	Vec3 playerCenter = _colliderA->GetOwner()->Transform()->GetRelativePos();
	Vec3 playerExtents = _colliderA->GetOwner()->Collider2D()->GetOffsetScale() / 2.f;
	Vec3 groundCenter = _colliderB->GetOwner()->Transform()->GetRelativePos();
	Vec3 groundExtents = _colliderB->GetOwner()->Collider2D()->GetOffsetScale() / 2.f;;

	// Calculate the overlap between the player and the ground
	float overlapX = (playerExtents.x + groundExtents.x) - abs(playerCenter.x - groundCenter.x);
	float overlapY = (playerExtents.y + groundExtents.y) - abs(playerCenter.y - groundCenter.y);

	// If there's an overlap, move the player out of the ground
	if (overlapX > 0 && overlapY > 0)
	{
		// Move the player by the minimum overlap in the y-axis direction
		Vec3 playerPos = _colliderA->GetOwner()->Transform()->GetRelativePos();
		playerPos.y += overlapY;
		_colliderA->GetOwner()->Transform()->SetRelativePos(playerPos);
	}
}
