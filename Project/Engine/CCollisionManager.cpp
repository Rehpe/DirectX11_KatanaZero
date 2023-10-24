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
	// �� ���̾��� �浹 ���θ� ��Ʈ�������� �����Ѵ�.

	UINT iRow = (UINT)_left;
	UINT iCol = (UINT)_right;

	// �浹 ��Ʈ�������� �׻� ��(iCol)�� ��(iRow) ���� ũ�� �����Ѵ�. (��Ʈ������ ������� �ﰢ�� ���)

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

			// iRow ���̾�� iCol ���̾�� ���� �浹�˻縦 �����Ѵ�.
			CollisionBtwLayer(pLevel->GetLayer(iRow), pLevel->GetLayer(iCol));
		}
	}
}

void CCollisionManager::CollisionBtwLayer(CLayer* _LeftLayer, CLayer* _RightLayer)
{
	const vector<CGameObject*>& vecLeft = _LeftLayer->GetObjects();
	const vector<CGameObject*>& vecRight = _RightLayer->GetObjects();

	// �� ���̾ ���� ���̾���
	if (_LeftLayer == _RightLayer)
	{
		// ���� �ϳ��� ������Ʈ(�ڱ��ڽ�) �浹 �˻縦 ���ϱ� ���� �˻��ϴ� j���� i+1�� �д�
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
	// �� ������Ʈ �� �ϳ��� �浹ü ������Ʈ�� ���ų� ��Ȱ��ȭ�� ����
	if (!(_LeftObject->Collider2D() && _RightObject->Collider2D())
		|| !(_LeftObject->Collider2D()->IsColliderActivate() && _RightObject->Collider2D()->IsColliderActivate()))
		return;

	// �浹ü ID ����
	CollisionID id = {};
	id.LeftID = _LeftObject->Collider2D()->GetID();
	id.RightID = _RightObject->Collider2D()->GetID();

	// �浹ü ID �ʿ��� �ش� �浹ü �˻�
	map<UINT_PTR, bool>::iterator iter = m_mapColID.find(id.id);
	if (iter == m_mapColID.end())
	{
		m_mapColID.insert(make_pair(id.id, false));		// �ʿ� �浹ü ����� ���ٸ� �浹ID, �����浹����: ������ ���
		iter = m_mapColID.find(id.id);
	}
	
	// �� �� �ϳ��� ���� ���� ���¶��(Dead ����)
	bool bDead = false;
	if (_LeftObject->IsDead() || _RightObject->IsDead())
	{
		bDead = true;
	}
	// �� �浹ü�� ���� �浹 ������ Ȯ��
	if (CollisionBtwCollider(_LeftObject->Collider2D(), _RightObject->Collider2D()))
	{
		if (bDead && iter->second)
		{
			// ������ �浹�� ���� �ִµ�, �������� ������Ʈ�� �ִٸ� EndOverlap ȣ��
			_LeftObject->Collider2D()->EndOverlap(_RightObject->Collider2D());
			_RightObject->Collider2D()->EndOverlap(_LeftObject->Collider2D());
		}
		else if (iter->second)
		{
			// ��� �浹 �� (On)
			_LeftObject->Collider2D()->OnOverlap(_RightObject->Collider2D());
			_RightObject->Collider2D()->OnOverlap(_LeftObject->Collider2D());
		}
		else
		{
			// �̹� �����ӿ� ù �浹 (Begin)
			if (!bDead)		// �� �� �ϳ��� �������� ���¶�� �浹�� �����Ѵ�
			{
				_LeftObject->Collider2D()->BeginOverlap(_RightObject->Collider2D());
				_RightObject->Collider2D()->BeginOverlap(_LeftObject->Collider2D());
				iter->second = true;	//�浹 ��� ����
			}
		}
	}

	else
	{
		// �浹 ���� (End)
		if (iter->second)
		{
			_LeftObject->Collider2D()->EndOverlap(_RightObject->Collider2D());
			_RightObject->Collider2D()->EndOverlap(_LeftObject->Collider2D());
			iter->second = false;	// �浹 ���� ����
		}
	}

}

bool CCollisionManager::CollisionBtwCollider(CCollider2D* _pLeft, CCollider2D* _pRight)
{
	if (_pLeft->GetCollider2DType() == COLLIDER2D_TYPE::RECT && _pRight->GetCollider2DType() == COLLIDER2D_TYPE::RECT)
	{
		// �� �ڽ� �浹ü�� �������� �浹 �˻� ����

		// 0 -- 1
		// |    |
		// 3 -- 2

		// RectMesh���� ��� ����ó��, ���� 1�� �簢���� ����� ������ 4���� �����Ѵ�. ���� ���� �����¿� 0.5�� �������� ������.
		Vec3 arrLocal[4] =
		{
			Vec3(-0.5f, 0.5f, 0.f),
			Vec3(0.5f, 0.5f, 0.f),
			Vec3(0.5f, -0.5f, 0.f),
			Vec3(-0.5f, -0.5f, 0.f),
		};

		// ���簢���� 4�� �� 2���� ���� �����ϹǷ�, 4���� �� �� 2���� ������ �˻��Ѵ�. 
		// �̶� �˻��� ���ʹ� ������ ���� 0->1 ����, 0->3 �����̴�.

		// �� �浹ü�� �� ǥ�� ���� 2���� ���Ѵ�. �� �簢���� 0, 1, 3�� ���� ���� �浹ü�� ��������� ���� ��������� ã��,
		// 1�� ������ 0�� ���� ���� 0->1 ���͸�, 3�� ������ 0�� ���� ���� 0->3 ���͸� ã�´�.

		Vec3 arrProj[4] = {};

		arrProj[0] = XMVector3TransformCoord(arrLocal[1], _pLeft->GetColliderWorldMat()) - XMVector3TransformCoord(arrLocal[0], _pLeft->GetColliderWorldMat());
		arrProj[1] = XMVector3TransformCoord(arrLocal[3], _pLeft->GetColliderWorldMat()) - XMVector3TransformCoord(arrLocal[0], _pLeft->GetColliderWorldMat());

		arrProj[2] = XMVector3TransformCoord(arrLocal[1], _pRight->GetColliderWorldMat()) - XMVector3TransformCoord(arrLocal[0], _pRight->GetColliderWorldMat());
		arrProj[3] = XMVector3TransformCoord(arrLocal[3], _pRight->GetColliderWorldMat()) - XMVector3TransformCoord(arrLocal[0], _pRight->GetColliderWorldMat());

		// �� �浹ü�� �߽������� ���͸� ���Ѵ�. �߽����� �� �浹ü�� ��������� �������� �� ���ÿ��� (0,0)�̾��� ���̹Ƿ�,
		// 0,0�� ������ ��������� ���� �� ���ָ� �߽������� ���Ͱ� ���´�.

		Vec3 vCenter = XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), _pRight->GetColliderWorldMat()) - XMVector3TransformCoord(Vec3(0.f, 0.f, 0.f), _pLeft->GetColliderWorldMat());


		// �и��� �׽�Ʈ
		for (int i = 0; i < 4; ++i)
		{
			// �� 4���� �и��� �׽�Ʈ�� �����Ѵ�. ���� ������ ������ ���ʹ� normalize�Ѵ�.
			Vec3 vProj = arrProj[i];
			vProj.Normalize();

			// 4���� ǥ����͸� ������ ���������� ������Ų �Ÿ��� ���� / 2
			float fProjDist = 0.f;
			for (int j = 0; j < 4; ++j)
			{
				fProjDist += fabsf(arrProj[j].Dot(vProj));
			}
			fProjDist /= 2.f;

			// �߽������� ���͸� ������ ���������� ������Ų �Ÿ�
			float fCenter = fabsf(vCenter.Dot(vProj));

			// 4�� ���� �����Ÿ��� 2�� ���� ���� �߽����� ������ �Ÿ����� ª�ٸ�, �� ������ �и��� �� �ִ� ���� �ִٴ� ��
			// ��, �� ������ �浹���� �ʴ´ٶ�� ��

			if (fProjDist < fCenter)
				return false;
		}

		// 4���� �˻� �� �� ���̶� false�� ������ �浹 ����, 4�� �˻� ��� �ش����� �ʴ´ٸ� true�� ���� ���̴�.
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
		// ������ ���̰� 0����, ���� �Ÿ��� ���� ������ ������ ������ �Ÿ�
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
