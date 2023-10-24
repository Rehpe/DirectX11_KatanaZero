#include "pch.h"
#include "CCollider2D.h"

#include "CScript.h"
#include "component.h"
#include "CLevel.h"
#include "CRenderManager.h"
#include "CCollisionManager.h"

CCollider2D::CCollider2D()
	:CComponent(COMPONENT_TYPE::COLLIDER2D)
	, m_bColliderActivate(true)
	, m_Shape(COLLIDER2D_TYPE::RECT)		// ����Ʈ ����� �簢��
	, m_bAbsolute(false)
	, m_iCollisionCount(0)
	, m_vStartPos(Vec3(0,0,0))
	, m_vEndPos(Vec3(0, 0, 0))
{
}

CCollider2D::~CCollider2D()
{
}

void CCollider2D::finaltick()
{
	if (!m_bColliderActivate)
		return;

	// �浹 ī��Ʈ�� ������� Assert
	assert(0 <= m_iCollisionCount);

	if (COLLIDER2D_TYPE::RECT == m_Shape)
	{
		// ==== �浹ü(�ڽ�)�� ���� ��� ���ϱ� ===
		// �浹ü�� ũ�����
		m_matCollider2D = XMMatrixScaling(m_vOffsetScale.x, m_vOffsetScale.y, m_vOffsetScale.z);

		// �浹ü�� ȸ���� ����

		// �浹ü�� �̵���� ���ؼ� ������Ŀ� ���Ѵ�
		m_matCollider2D *= XMMatrixTranslation(m_vOffsetPos.x, m_vOffsetPos.y, m_vOffsetPos.z);

		// �浹ü ���� ������Ʈ�� ���� ��� �޾ƿ�
		const Matrix& matWorld = Transform()->GetWorldMat();

		if (m_bAbsolute)
		{
			// ���밪 �ɼ��� ��� �θ�(�浹ü ����) ũ�⿡ ������ ���� �ʾƾ� �ϹǷ� �θ� ũ������� ���� ���Ѵ�.
			Matrix matParentScaleInv = XMMatrixInverse(nullptr, Transform()->GetWorldScaleMat());
			m_matCollider2D = m_matCollider2D * matParentScaleInv * matWorld;
		}
		else
		{
			// �浹ü�� ������� = ������ ������� x �θ�(�浹ü ����)�� �������
			m_matCollider2D *= matWorld;
		}
	}

	if (COLLIDER2D_TYPE::LINE == m_Shape) 
	{
		// �浹ü ������ķ� �ʱ�ȭ
		m_matCollider2D = XMMatrixIdentity();

		// �浹ü ���� ������Ʈ�� ���� ��� �޾ƿ�
		const Matrix& matWorld = Transform()->GetWorldMat();

		if (m_bAbsolute) 
		{
			/// ���밪 �ɼ��� ��� �θ�(�浹ü ����) ũ�⿡ ������ ���� �ʾƾ� �ϹǷ� �θ� ũ������� ���� ���Ѵ�.
			Matrix matParentScaleInv = XMMatrixInverse(nullptr, Transform()->GetWorldScaleMat());
			m_vStartPos = XMVector3TransformCoord(m_vStartPos, matParentScaleInv * matWorld);
			m_vEndPos = XMVector3TransformCoord(m_vEndPos, matParentScaleInv * matWorld);
		}
		else 
		{
			// ���밪 �ɼ��� �ƴ� ��� �ڽ��� ��ġ�� ��������� ���Ѵ�.
			m_vStartPos = XMVector3TransformCoord(m_vStartPos, matWorld);
			m_vEndPos = XMVector3TransformCoord(m_vEndPos, matWorld);
		}
	}

	// Collider Debug ���
	if(!(CCollisionManager::GetInst()->IsColliderDebugMode()))
		return;
	
	// �浹 ���� ��� ������, �浹 ������ ���� ��� �ʷϻ����� DebugShape Color ����
	Vec4 vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	
	if (0 < m_iCollisionCount)				//�浹 ���� ��� 
		vColor = Vec4(1.f, 0.f, 0.f, 1.f);

	if (CLevelManager::GetInst()->GetCurLevel()->GetState() == LEVEL_STATE::PLAY)
	{
		// ���� ī�޶��� �̵����� �޾� �ݴ�� �̵��ϴ� ����� �����.
		CCamera* pMainCam = CRenderManager::GetInst()->GetMainCam();
		if (pMainCam == nullptr)
			return;

		Vec3 CameraPos = pMainCam->GetOwner()->Transform()->GetRelativePos();
		Matrix transInvCamMat = XMMatrixTranslation(-CameraPos.x, -CameraPos.y, 0.f);

		if (COLLIDER2D_TYPE::CIRCLE == m_Shape)
			DrawDebugCircle(m_matCollider2D * transInvCamMat, vColor, 0.f);
		else if (COLLIDER2D_TYPE::RECT == m_Shape)
			DrawDebugRect(m_matCollider2D * transInvCamMat, vColor, 0.f);
		else // ���� �浹ü�� ���
		{
			Vec3 StartPos = XMVector3TransformCoord(m_vStartPos, transInvCamMat);
			Vec3 EndPos = XMVector3TransformCoord(m_vEndPos, transInvCamMat);
			DrawDebugLine(StartPos, EndPos, vColor, 0.f);
		}

	}
	else
	{// DebugShape ��û
		if (COLLIDER2D_TYPE::CIRCLE == m_Shape)
		{
			// �� finaltick���� �����Ƿ� time�� 0�ʿ��� ��������, ���� ������Ʈ ������ �ٷ� �������
			// �浹ü�� �׸��� ���� DrawDebugShape�� ȣ���Ѵٸ� Time�� 0���� �ص� �浹ü�� �� tick���� 
			// �Լ��� ��� ȣ���ϹǷ� ��� ������ ���̴�.
			DrawDebugCircle(m_matCollider2D, vColor, 0.f);
		}
		else if (COLLIDER2D_TYPE::RECT == m_Shape)
		{
			DrawDebugRect(m_matCollider2D, vColor, 0.f);
		}
		else // ���� �浹ü�� ���
		{
			DrawDebugLine(m_vStartPos, m_vEndPos, vColor, 0.f);
		}
	}
}

Vec3 CCollider2D::GetColliderWorldPos()
{
	if (GetOwner()->GetParent() != nullptr)
	{
		Vec3 localPos = GetOwner()->Transform()->GetRelativePos();
		Vec4 localPosWithW(localPos.x, localPos.y, localPos.z, 1.0f);
		Vec4 worldPosWithW = XMVector4Transform(localPosWithW, m_matCollider2D);
		return Vec3(worldPosWithW.x, worldPosWithW.y, worldPosWithW.z);
	}
	else
	{
		return GetOffsetPos();
	}
}

void CCollider2D::BeginOverlap(CCollider2D* _Other)
{
	m_iCollisionCount++;

	// ������ ��ũ��Ʈ�� ���� �޾ƿ� �� ��ũ��Ʈ�� �������̵��� BeginOverlap ��� ȣ��
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->BeginOverlap(_Other);
	}
}

void CCollider2D::OnOverlap(CCollider2D* _Other)
{
	// ������ ��ũ��Ʈ�� ���� �޾ƿ� �� ��ũ��Ʈ�� �������̵��� OnOverlap ��� ȣ��
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->OnOverlap(_Other);
	}

}

void CCollider2D::EndOverlap(CCollider2D* _Other)
{
	m_iCollisionCount--;

	// ������ ��ũ��Ʈ�� ���� �޾ƿ� �� ��ũ��Ʈ�� �������̵��� EndOverlap ��� ȣ��
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->EndOverlap(_Other);
	}
}

void CCollider2D::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_Shape, sizeof(UINT), 1, _File);
	fwrite(&m_bAbsolute, sizeof(bool), 1, _File);
	if (m_Shape == COLLIDER2D_TYPE::RECT)
	{
		fwrite(&m_vOffsetPos, sizeof(Vec3), 1, _File);
		fwrite(&m_vOffsetScale, sizeof(Vec3), 1, _File);
	}
	else if (m_Shape == COLLIDER2D_TYPE::LINE)
	{
		fwrite(&m_vStartPos, sizeof(Vec3), 1, _File);
		fwrite(&m_vEndPos, sizeof(Vec3), 1, _File);
	}
}

void CCollider2D::LoadFromLevelFile(FILE* _File)
{
	fread(&m_Shape, sizeof(UINT), 1, _File);
	fread(&m_bAbsolute, sizeof(bool), 1, _File);
	if (m_Shape == COLLIDER2D_TYPE::RECT)
	{
		fread(&m_vOffsetPos, sizeof(Vec3), 1, _File);
		fread(&m_vOffsetScale, sizeof(Vec3), 1, _File);
	}
	else if (m_Shape == COLLIDER2D_TYPE::LINE)
	{
		fread(&m_vStartPos, sizeof(Vec3), 1, _File);
		fread(&m_vEndPos, sizeof(Vec3), 1, _File);
	}
}
