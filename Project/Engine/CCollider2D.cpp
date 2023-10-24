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
	, m_Shape(COLLIDER2D_TYPE::RECT)		// 디폴트 모양은 사각형
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

	// 충돌 카운트가 음수라면 Assert
	assert(0 <= m_iCollisionCount);

	if (COLLIDER2D_TYPE::RECT == m_Shape)
	{
		// ==== 충돌체(자신)의 월드 행렬 구하기 ===
		// 충돌체의 크기행렬
		m_matCollider2D = XMMatrixScaling(m_vOffsetScale.x, m_vOffsetScale.y, m_vOffsetScale.z);

		// 충돌체의 회전은 없음

		// 충돌체의 이동행렬 구해서 월드행렬에 곱한다
		m_matCollider2D *= XMMatrixTranslation(m_vOffsetPos.x, m_vOffsetPos.y, m_vOffsetPos.z);

		// 충돌체 주인 오브젝트의 월드 행렬 받아옴
		const Matrix& matWorld = Transform()->GetWorldMat();

		if (m_bAbsolute)
		{
			// 절대값 옵션일 경우 부모(충돌체 주인) 크기에 영향을 받지 않아야 하므로 부모 크기행렬의 역을 곱한다.
			Matrix matParentScaleInv = XMMatrixInverse(nullptr, Transform()->GetWorldScaleMat());
			m_matCollider2D = m_matCollider2D * matParentScaleInv * matWorld;
		}
		else
		{
			// 충돌체의 월드행렬 = 본인의 월드행렬 x 부모(충돌체 주인)의 월드행렬
			m_matCollider2D *= matWorld;
		}
	}

	if (COLLIDER2D_TYPE::LINE == m_Shape) 
	{
		// 충돌체 단위행렬로 초기화
		m_matCollider2D = XMMatrixIdentity();

		// 충돌체 주인 오브젝트의 월드 행렬 받아옴
		const Matrix& matWorld = Transform()->GetWorldMat();

		if (m_bAbsolute) 
		{
			/// 절대값 옵션일 경우 부모(충돌체 주인) 크기에 영향을 받지 않아야 하므로 부모 크기행렬의 역을 곱한다.
			Matrix matParentScaleInv = XMMatrixInverse(nullptr, Transform()->GetWorldScaleMat());
			m_vStartPos = XMVector3TransformCoord(m_vStartPos, matParentScaleInv * matWorld);
			m_vEndPos = XMVector3TransformCoord(m_vEndPos, matParentScaleInv * matWorld);
		}
		else 
		{
			// 절대값 옵션이 아닐 경우 자신의 위치에 월드행렬을 곱한다.
			m_vStartPos = XMVector3TransformCoord(m_vStartPos, matWorld);
			m_vEndPos = XMVector3TransformCoord(m_vEndPos, matWorld);
		}
	}

	// Collider Debug 모드
	if(!(CCollisionManager::GetInst()->IsColliderDebugMode()))
		return;
	
	// 충돌 중인 경우 빨간색, 충돌 중이지 않은 경우 초록색으로 DebugShape Color 설정
	Vec4 vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	
	if (0 < m_iCollisionCount)				//충돌 중인 경우 
		vColor = Vec4(1.f, 0.f, 0.f, 1.f);

	if (CLevelManager::GetInst()->GetCurLevel()->GetState() == LEVEL_STATE::PLAY)
	{
		// 메인 카메라의 이동값을 받아 반대로 이동하는 행렬을 만든다.
		CCamera* pMainCam = CRenderManager::GetInst()->GetMainCam();
		if (pMainCam == nullptr)
			return;

		Vec3 CameraPos = pMainCam->GetOwner()->Transform()->GetRelativePos();
		Matrix transInvCamMat = XMMatrixTranslation(-CameraPos.x, -CameraPos.y, 0.f);

		if (COLLIDER2D_TYPE::CIRCLE == m_Shape)
			DrawDebugCircle(m_matCollider2D * transInvCamMat, vColor, 0.f);
		else if (COLLIDER2D_TYPE::RECT == m_Shape)
			DrawDebugRect(m_matCollider2D * transInvCamMat, vColor, 0.f);
		else // 라인 충돌체일 경우
		{
			Vec3 StartPos = XMVector3TransformCoord(m_vStartPos, transInvCamMat);
			Vec3 EndPos = XMVector3TransformCoord(m_vEndPos, transInvCamMat);
			DrawDebugLine(StartPos, EndPos, vColor, 0.f);
		}

	}
	else
	{// DebugShape 요청
		if (COLLIDER2D_TYPE::CIRCLE == m_Shape)
		{
			// 매 finaltick마다 들어오므로 time이 0초여도 보여지며, 원본 오브젝트 삭제시 바로 사라진다
			// 충돌체를 그리기 위해 DrawDebugShape를 호출한다면 Time을 0으로 해도 충돌체가 매 tick마다 
			// 함수를 계속 호출하므로 계속 보여질 것이다.
			DrawDebugCircle(m_matCollider2D, vColor, 0.f);
		}
		else if (COLLIDER2D_TYPE::RECT == m_Shape)
		{
			DrawDebugRect(m_matCollider2D, vColor, 0.f);
		}
		else // 라인 충돌체일 경우
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

	// 본인의 스크립트를 전부 받아온 후 스크립트에 오버라이딩된 BeginOverlap 모두 호출
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->BeginOverlap(_Other);
	}
}

void CCollider2D::OnOverlap(CCollider2D* _Other)
{
	// 본인의 스크립트를 전부 받아온 후 스크립트에 오버라이딩된 OnOverlap 모두 호출
	const vector<CScript*>& vecScript = GetOwner()->GetScripts();
	for (size_t i = 0; i < vecScript.size(); ++i)
	{
		vecScript[i]->OnOverlap(_Other);
	}

}

void CCollider2D::EndOverlap(CCollider2D* _Other)
{
	m_iCollisionCount--;

	// 본인의 스크립트를 전부 받아온 후 스크립트에 오버라이딩된 EndOverlap 모두 호출
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
