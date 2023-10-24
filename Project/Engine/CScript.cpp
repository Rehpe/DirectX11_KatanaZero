#include "pch.h"
#include "CScript.h"

CScript::CScript(UINT _ScriptType)
	: CComponent(COMPONENT_TYPE::SCRIPT)
	, m_iScriptType(_ScriptType)
{
}

CScript::~CScript()
{
}

void CScript::AddScriptParam(SCRIPT_PARAM eParam, void* _pData, const string& _Desc)
{
	m_vecParam.push_back(tScriptParam{ eParam , _pData , _Desc });
}

Vec3 CScript::CalculateOverlapAmount(CCollider2D* _colliderA, CCollider2D* _colliderB)
{
	Vec3 posA = _colliderA->GetOwner()->Transform()->GetRelativePos();
	Vec3 posB = _colliderB->GetOwner()->Transform()->GetRelativePos();
	Vec3 extentsA = _colliderA->GetOffsetScale() / 2.f;
	Vec3 extentsB = _colliderB->GetOffsetScale() / 2.f;

	// Calculate the overlapping amounts for each axis
	float xOverlap = (extentsA.x + extentsB.x) - abs(posA.x - posB.x);
	float yOverlap = (extentsA.y + extentsB.y) - abs(posA.y - posB.y);

	// Return the overlap amounts
	return Vec3(xOverlap, yOverlap, 0);
}

CollisionDir CScript::GetCollisionDirX(CCollider2D* _colliderA, CCollider2D* _colliderB)
{
	// A 기준, A가 B에 충돌했을 경우 B의 좌/우 중 어느 쪽에서 충돌했는지 반환

	Vec3 posA = _colliderA->GetOwner()->Transform()->GetRelativePos();
	Vec3 posB = _colliderB->GetOwner()->Transform()->GetRelativePos();
	Vec3 deltaPos = posA - posB;

	if (deltaPos.x > 0)
	{
		return CollisionDir::RIGHT;
	}
	else
	{
		return CollisionDir::LEFT;
	}
}

CollisionDir CScript::GetCollisionDirY(CCollider2D* _colliderA, CCollider2D* _colliderB)
{
	// A 기준, A가 B에 충돌했을 경우 B의 상/하 중 어느 쪽에서 충돌했는지 반환
	Vec3 posA = _colliderA->GetOwner()->Transform()->GetRelativePos();
	Vec3 posB = _colliderB->GetOwner()->Transform()->GetRelativePos();
	Vec3 deltaPos = posA - posB;

	if (deltaPos.y > 0)
	{
		return CollisionDir::UP;
	}
	else
	{
		return CollisionDir::DOWN;
	}
}
