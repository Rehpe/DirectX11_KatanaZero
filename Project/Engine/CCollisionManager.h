#pragma once
#include "CSingleton.h"

// 두 충돌체간의 충돌 여부와 충돌 기록을 관리하는 클래스

class CLayer;
class CCollider2D;

union CollisionID
{
	struct
	{
		UINT LeftID;
		UINT RightID;
	};

	UINT_PTR id;
};

class CCollisionManager :
    public CSingleton<CCollisionManager>
{
    SINGLETON(CCollisionManager);

private:
	UINT					m_matrix[MAX_LAYER];		// 레이어가 32개이므로 UINT 타입으로 잡아준다
	map<UINT_PTR, bool>		m_mapColID;

	bool					m_bColliderDebugMode;

public:
	void LayerCheck(UINT _left, UINT _right);
	void LayerCheck(const wstring& _strLeftLayer, const wstring& _strRightLayer);

	void Clear()
	{
		memset(m_matrix, 0, sizeof(UINT) * MAX_LAYER);
	}

public:
	void tick();

private:
	void CollisionBtwLayer(CLayer* _LeftLayer, CLayer* _RightLayer);
	void CollisionBtwObject(CGameObject* _LeftObject, CGameObject* _RightObject);
	bool CollisionBtwCollider(CCollider2D* _pLeft, CCollider2D* _pRight);

public:
	bool IsLayerCollisionEnabled(UINT row, UINT col);
	void DisableLayerCollision(UINT row, UINT col);

	bool IsColliderDebugMode() { return m_bColliderDebugMode; }

private:
	// 점과 선분 사이의 길이를 반환하는 함수
	float PointToLineSegmentDistance(const Vec3& _Point, const Vec3& _LineStart, const Vec3& _LineEnd);
	
	Vec3 ClosestPointOnLineSegment(const Vec3& startPos, const Vec3& endPos, const Vec3& boxCentner);
};

