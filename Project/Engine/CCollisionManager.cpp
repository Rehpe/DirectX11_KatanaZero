#include "pch.h"
#include "CCollisionManager.h"

#include "CLevel.h"
#include "CLayer.h"
#include "CLevelManager.h"
#include "CGameObject.h"
#include "CCollider2D.h"
#include "CTransform.h"
#include "CKeyManager.h"

#include <algorithm>

CCollisionManager::CCollisionManager()
	: m_matrix{}
	, m_bColliderDebugMode(false)
{
}

CCollisionManager::~CCollisionManager()
{
}

void CCollisionManager::LayerCheck(UINT _left, UINT _right)
{
	// 두 레이어의 충돌 여부를 비트연산으로 설정한다.

	UINT iRow = (UINT)_left;
	UINT iCol = (UINT)_right;

	// 충돌 매트릭스에서 항상 열(iCol)이 행(iRow) 보다 크게 설정한다. (매트릭스의 우측상단 삼각형 사용)

	if (iRow > iCol)
	{
		UINT iTemp = iCol;
		iCol = iRow;
		iRow = iTemp;
	}

	m_matrix[iRow] |= (1 << iCol);
}

void CCollisionManager::LayerCheck(const wstring& _strLeftLayer, const wstring& _strRightLayer)
{
	CLevel* pCurLevel = CLevelManager::GetInst()->GetCurLevel();

	CLayer* pLeft = pCurLevel->FindLayerByName(_strLeftLayer);
	CLayer* pRight = pCurLevel->FindLayerByName(_strRightLayer);

	LayerCheck(pLeft->GetLayerIndex(), pRight->GetLayerIndex());
}

void CCollisionManager::tick()
{
	if (KEY_TAP(KEY::P))
	{
		if (m_bColliderDebugMode)
			m_bColliderDebugMode = false;
		else
			m_bColliderDebugMode = true;
	}

	CLevel* pLevel = CLevelManager::GetInst()->GetCurLevel();

	for (UINT iRow = 0; iRow < MAX_LAYER; ++iRow)
	{
		for (UINT iCol = iRow; iCol < MAX_LAYER; ++iCol)
		{
			if (!(m_matrix[iRow] & (1 << iCol)))
				continue;

			// iRow 레이어와 iCol 레이어는 서로 충돌검사를 진행한다.
			CollisionBtwLayer(pLevel->GetLayer(iRow), pLevel->GetLayer(iCol));
		}
	}
}

void CCollisionManager::CollisionBtwLayer(CLayer* _LeftLayer, CLayer* _RightLayer)
{
	const vector<CGameObject*>& vecLeft = _LeftLayer->GetObjects();
	const vector<CGameObject*>& vecRight = _RightLayer->GetObjects();

	// 두 레이어가 같은 레이어라면
	if (_LeftLayer == _RightLayer)
	{
		// 같은 하나의 오브젝트(자기자신) 충돌 검사를 피하기 위해 검사하는 j값을 i+1로 둔다
		for (size_t i = 0; i < vecLeft.size(); ++i)
		{
			for (size_t j = i + 1; j < vecRight.size(); ++j)
			{
				CollisionBtwObject(vecLeft[i], vecRight[j]);
			}
		}
	}
	else
	{
		for (size_t i = 0; i < vecLeft.size(); ++i)
		{
			for (size_t j = 0; j < vecRight.size(); ++j)
			{
				CollisionBtwObject(vecLeft[i], vecRight[j]);
			}
		}
	}
}

void CCollisionManager::CollisionBtwObject(CGameObject* _LeftObject, CGameObject* _RightObject)
{
	// 두 오브젝트 중 하나라도 충돌체 컴포넌트가 없거나 비활성화면 리턴
	if (!(_LeftObject->Collider2D() && _RightObject->Collider2D())
		|| !(_LeftObject->Collider2D()->IsColliderActivate() && _RightObject->Collider2D()->IsColliderActivate()))
		return;

	// 충돌체 ID 생성
	CollisionID id = {};
	id.LeftID = _LeftObject->Collider2D()->GetID();
	id.RightID = _RightObject->Collider2D()->GetID();

	// 충돌체 ID 맵에서 해당 충돌체 검색
	map<UINT_PTR, bool>::iterator iter = m_mapColID.find(id.id);
	if (iter == m_mapColID.end())
	{
		m_mapColID.insert(make_pair(id.id, false));		// 맵에 충돌체 기록이 없다면 충돌ID, 이전충돌여부: 없음을 등록
		iter = m_mapColID.find(id.id);
	}
	
	// 둘 중 하나라도 삭제 예정 상태라면(Dead 상태)
	bool bDead = false;
	if (_LeftObject->IsDead() || _RightObject->IsDead())
	{
		bDead = true;
	}
	// 두 충돌체가 지금 충돌 중인지 확인
	if (CollisionBtwCollider(_LeftObject->Collider2D(), _RightObject->Collider2D()))
	{
		if (bDead && iter->second)
		{
			// 이전에 충돌한 적이 있는데, 삭제예정 오브젝트가 있다면 EndOverlap 호출
			_LeftObject->Collider2D()->EndOverlap(_RightObject->Collider2D());
			_RightObject->Collider2D()->EndOverlap(_LeftObject->Collider2D());
		}
		else if (iter->second)
		{
			// 계속 충돌 중 (On)
			_LeftObject->Collider2D()->OnOverlap(_RightObject->Collider2D());
			_RightObject->Collider2D()->OnOverlap(_LeftObject->Collider2D());
		}
		else
		{
			// 이번 프레임에 첫 충돌 (Begin)
			if (!bDead)		// 둘 중 하나라도 삭제예정 상태라면 충돌을 무시한다
			{
				_LeftObject->Collider2D()->BeginOverlap(_RightObject->Collider2D());
				_RightObject->Collider2D()->BeginOverlap(_LeftObject->Collider2D());
				iter->second = true;	//충돌 기록 갱신
			}
		}
	}

	else
	{
		// 충돌 해제 (End)
		if (iter->second)
		{
			_LeftObject->Collider2D()->EndOverlap(_RightObject->Collider2D());
			_RightObject->Collider2D()->EndOverlap(_LeftObject->Collider2D());
			iter->second = false;	// 충돌 해제 갱신
		}
	}

}

bool CCollisionManager::CollisionBtwCollider(CCollider2D* _pLeft, CCollider2D* _pRight)
{
	if (_pLeft->GetCollider2DType() == COLLIDER2D_TYPE::RECT && _pRight->GetCollider2DType() == COLLIDER2D_TYPE::RECT)
	{
		// 두 박스 충돌체의 실질적인 충돌 검사 진행

		// 0 -- 1
		// |    |
		// 3 -- 2

		// RectMesh에서 썼던 설정처럼, 길이 1인 사각형을 만드는 꼭지점 4개를 지정한다. 원점 기준 상하좌우 0.5의 반지름을 가진다.
		Vec3 arrLocal[4] =
		{
			Vec3(-0.5f, 0.5f, 0.f),
			Vec3(0.5f, 0.5f, 0.f),
			Vec3(0.5f, -0.5f, 0.f),
			Vec3(-0.5f, -0.5f, 0.f),
		};

		// 직사각형은 4개 중 2개의 선이 평행하므로, 4개의 선 중 2개만 축으로 검사한다. 
		// 이때 검사할 벡터는 꼭지점 기준 0->1 벡터, 0->3 벡터이다.

		// 두 충돌체의 각 표면 벡터 2개씩 구한다. 각 사각형의 0, 1, 3번 점에 각의 충돌체의 월드행렬을 곱해 월드행렬을 찾고,
		// 1번 점에서 0번 점을 빼서 0->1 벡터를, 3번 점에서 0번 점을 빼서 0->3 벡터를 찾는다.

		Vec3 arrProj[4] = {};

		arrProj[0] = XMVector3TransformCoord(arrLocal[1], _pLeft->GetColliderWorldMat()) - XMVector3TransformCoord(arrLocal[0], _pLeft->GetColliderWorldMat());
		arrProj[1] = XMVector3TransformCoord(arrLocal[3], _pLeft->GetColliderWorldMat()) - XMVector3TransformCoord(arrLocal[0], _pLeft->GetColliderWorldMat());

		arrProj[2] = XMVector3TransformCoord(arrLocal[1], _pRight->GetColliderWorldMat()) - XMVector3TransformCoord(arrLocal[0], _pRight->GetColliderWorldMat());
		arrProj[3] = XMVector3TransformCoord(arrLocal[3], _pRight->GetColliderWorldMat()) - XMVector3TransformCoord(arrLocal[0], _pRight->GetColliderWorldMat());

		// 두 충돌체의 중심점간의 벡터를 구한다. 중심점은 각 충돌체가 월드행렬이 곱해지기 전 로컬에서 (0,0)이었던 점이므로,
		// 0,0에 각각의 월드행렬을 곱한 뒤 빼주면 중심점간의 벡터가 나온다.

		Vec3 vCenter = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), _pRight->GetColliderWorldMat()) - XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), _pLeft->GetColliderWorldMat());


		// 분리축 테스트
		for (int i = 0; i < 4; ++i)
		{
			// 총 4번의 분리축 테스트를 진행한다. 비교할 축으로 선정된 벡터는 normalize한다.
			Vec3 vProj = arrProj[i];
			vProj.Normalize();

			// 4개의 표면백터를 지정된 투영축으로 투영시킨 거리의 총합 / 2
			float fProjDist = 0.f;
			for (int j = 0; j < 4; ++j)
			{
				fProjDist += fabsf(arrProj[j].Dot(vProj));
			}
			fProjDist /= 2.f;

			// 중심점간의 벡터를 지정된 투영축으로 투영시킨 거리
			float fCenter = fabsf(vCenter.Dot(vProj));

			// 4개 축의 투영거리를 2로 나눈 것이 중심점을 투영한 거리보다 짧다면, 두 도형을 분리할 수 있는 축이 있다는 뜻
			// 즉, 두 도형은 충돌하지 않는다라는 뜻

			if (fProjDist < fCenter)
				return false;
		}

		// 4번의 검사 중 한 번이라도 false가 나오면 충돌 실패, 4번 검사 모두 해당하지 않는다면 true가 나올 것이다.
		return true;
	}

	if ((_pLeft->GetCollider2DType() == COLLIDER2D_TYPE::RECT && _pRight->GetCollider2DType() == COLLIDER2D_TYPE::LINE) ||
		(_pLeft->GetCollider2DType() == COLLIDER2D_TYPE::LINE && _pRight->GetCollider2DType() == COLLIDER2D_TYPE::RECT))
	{
		CCollider2D* lineCollider = (_pLeft->GetCollider2DType() == COLLIDER2D_TYPE::LINE) ? _pLeft : _pRight;
		CCollider2D* boxCollider = (_pLeft->GetCollider2DType() == COLLIDER2D_TYPE::RECT) ? _pLeft : _pRight;

		Vec3 boxPos = Vec3(boxCollider->GetOwner()->Transform()->GetRelativePos().x
						 , boxCollider->GetOwner()->Transform()->GetRelativePos().y
						 , 0.f) ;
		Vec3 boxExtent = boxCollider->GetOffsetScale() * 0.5f;
		Vec3 lineStartPos = lineCollider->GetStartPos();
		Vec3 lineEndPos = lineCollider->GetEndPos();

		Vec3 ClosestPoint = ClosestPointOnLineSegment(lineStartPos, lineEndPos, boxPos);
	
		bool pointInsideBox = (ClosestPoint.x >= boxPos.x - boxExtent.x) &&
								(ClosestPoint.x <= boxPos.x + boxExtent.x) &&
								(ClosestPoint.y >= boxPos.y - boxExtent.y) &&
								(ClosestPoint.y <= boxPos.y + boxExtent.y);

		return pointInsideBox;
	}

}

bool CCollisionManager::IsLayerCollisionEnabled(UINT row, UINT col)
{
	if (row > col)
	{
		UINT temp = col;
		col = row;
		row = temp;
	}

	UINT checkBit = (1 << col);
	return (m_matrix[row] & checkBit) != 0;
}

void CCollisionManager::DisableLayerCollision(UINT row, UINT col)
{
	if (row > col)
	{
		UINT temp = col;
		col = row;
		row = temp;
	}

	UINT clearBit = ~(1 << col);
	m_matrix[row] &= clearBit;
}

float CCollisionManager::PointToLineSegmentDistance(const Vec3& _Point, const Vec3& _LineStart, const Vec3& _LineEnd)
{
	Vec3 LineVector = _LineEnd - _LineStart;
	float LineLengthSquared = LineVector.Dot(LineVector);

	if (LineLengthSquared == 0.0f)
	{
		// 라인의 길이가 0으로, 따라서 거리는 점과 라인의 시작점 사이의 거리
		return (_Point - _LineStart).Length();
	}

	// Project the point onto the line segment
	float t = (_Point - _LineStart).Dot(_LineEnd - _LineStart) / LineLengthSquared;
	t = max(0.0f, min(1.0f, t));

	Vec3 projection = _LineStart + t * LineVector;
	return (_Point - projection).Length();
}

Vec3 CCollisionManager::ClosestPointOnLineSegment(const Vec3& startPos, const Vec3& endPos, const Vec3& boxCenter)
{
	Vec3 LineVector = endPos - startPos;
	float t = (boxCenter - startPos).Dot(LineVector) / LineVector.LengthSquared();

	t = max(0.0f, min(t, 1.0f));

	return startPos + t * LineVector;
}
