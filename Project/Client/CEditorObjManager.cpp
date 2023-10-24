#include "pch.h"
#include "CEditorObjManager.h"

#include "CGameObjectEX.h"
#include <Engine\component.h>
#include <Engine\CResourceManager.h>
#include <Engine\CRenderManager.h>
#include <Engine\CTimeManager.h>
#include <Engine\CKeyManager.h>
#include <Engine\CLevel.h>

#include <Script\CCameraMoveScript.h>


CEditorObjManager::CEditorObjManager()
	: m_DebugShape{}
{
}

CEditorObjManager::~CEditorObjManager()
{
	Safe_Del_Vec(m_vecEditorObj);
	Safe_Del_Array(m_DebugShape);
}

void CEditorObjManager::init()
{
	// 디버그 쉐이프 생성
	m_DebugShape[(UINT)SHAPE_TYPE::RECT] = new CGameObjectEX;
	m_DebugShape[(UINT)SHAPE_TYPE::RECT]->AddComponent(new CTransform);
	m_DebugShape[(UINT)SHAPE_TYPE::RECT]->AddComponent(new CMeshRender);
	m_DebugShape[(UINT)SHAPE_TYPE::RECT]->MeshRender()->SetMesh(CResourceManager::GetInst()->FindRes<CMesh>(L"RectMesh_Debug"));
	m_DebugShape[(UINT)SHAPE_TYPE::RECT]->MeshRender()->SetMaterial(CResourceManager::GetInst()->FindRes<CMaterial>(L"DebugShapeMtrl"));

	m_DebugShape[(UINT)SHAPE_TYPE::CIRCLE] = new CGameObjectEX;
	m_DebugShape[(UINT)SHAPE_TYPE::CIRCLE]->AddComponent(new CTransform);
	m_DebugShape[(UINT)SHAPE_TYPE::CIRCLE]->AddComponent(new CMeshRender);
	m_DebugShape[(UINT)SHAPE_TYPE::CIRCLE]->MeshRender()->SetMesh(CResourceManager::GetInst()->FindRes<CMesh>(L"CircleMesh_Debug"));
	m_DebugShape[(UINT)SHAPE_TYPE::CIRCLE]->MeshRender()->SetMaterial(CResourceManager::GetInst()->FindRes<CMaterial>(L"DebugShapeMtrl"));

	m_DebugShape[(UINT)SHAPE_TYPE::LINE] = new CGameObjectEX;
	m_DebugShape[(UINT)SHAPE_TYPE::LINE]->AddComponent(new CTransform);
	m_DebugShape[(UINT)SHAPE_TYPE::LINE]->AddComponent(new CMeshRender);
	m_DebugShape[(UINT)SHAPE_TYPE::LINE]->MeshRender()->SetMesh(CResourceManager::GetInst()->FindRes<CMesh>(L"LineMesh_Debug"));
	m_DebugShape[(UINT)SHAPE_TYPE::LINE]->MeshRender()->SetMaterial(CResourceManager::GetInst()->FindRes<CMaterial>(L"DebugShapeMtrl"));

	// Editor Camera 생성
	CGameObjectEX* pEditorCamObj = new CGameObjectEX;
	pEditorCamObj->AddComponent(new CTransform);
	pEditorCamObj->AddComponent(new CCamera);
	pEditorCamObj->AddComponent(new CCameraMoveScript);

	pEditorCamObj->Camera()->SetLayerMaskAll(true);

	m_vecEditorObj.push_back(pEditorCamObj);
	CRenderManager::GetInst()->RegisterEditorCamera(pEditorCamObj->Camera());

}

void CEditorObjManager::progress()
{
	// tick-render 전, RenderManager에게서 DebugShape 정보 가져오기
	vector<tDebugShapeInfo>& vecInfo = CRenderManager::GetInst()->GetDebugShapeInfo();
	
	// 받아온 DebugShape 정보를 멤버인 m_DebugShapeInfo에 그대로 복사
	m_DebugShapeInfo.insert(m_DebugShapeInfo.end(), vecInfo.begin(), vecInfo.end());
	
	// 받아온 지역변수 벡터는 clear
	vecInfo.clear();

	tick();
	render();
}

void CEditorObjManager::tick()
{
	// Editor Camera를 포함한 EditorObject는 EditorObjectManager가 tick, render 을 돌려준다. 
	for (size_t i = 0; i < m_vecEditorObj.size(); ++i)
	{
		m_vecEditorObj[i]->tick();
	}

	for (size_t i = 0; i < m_vecEditorObj.size(); ++i)
	{
		m_vecEditorObj[i]->finaltick();
	}
}

void CEditorObjManager::render()
{
	// Editor Camera를 포함한 EditorObject는 EditorObjectManager가 tick, render 을 돌려준다. 
	for (size_t i = 0; i < m_vecEditorObj.size(); ++i)
	{
		m_vecEditorObj[i]->render();
	}

	// DebugShape Render
	CGameObjectEX* pShapeObj = nullptr;

	// 지속시간이 만료된 Obj를 벡터에서 빼야하므로 iter을 사용한다.
	vector<tDebugShapeInfo>::iterator iter = m_DebugShapeInfo.begin();

	for (; iter != m_DebugShapeInfo.end();)
	{
		switch (iter->eShape)
		{
		case SHAPE_TYPE::RECT:
			pShapeObj = m_DebugShape[(UINT)SHAPE_TYPE::RECT];
			break;
		case SHAPE_TYPE::CIRCLE:
			pShapeObj = m_DebugShape[(UINT)SHAPE_TYPE::CIRCLE];
			break;
		case SHAPE_TYPE::LINE:
			pShapeObj = m_DebugShape[(UINT)SHAPE_TYPE::LINE];
			break;
		case SHAPE_TYPE::CUBE:
			//pShapeObj = m_DebugShape[(UINT)SHAPE_TYPE::CUBE];
			break;
		case SHAPE_TYPE::SPHERE:
			//pShapeObj = m_DebugShape[(UINT)SHAPE_TYPE::SPHERE];
			break;
		}

		// DebugShape의 행렬이 단위행렬이라면 -> 행렬이 아닌 로컬 위치,크기,회전 3개가 따로 들어왔다는 소리
		// 들어온 위치,크기,회전값을 SetRelative함수로 설정 후 finaltick을 돌면 행렬이 계산되어 나올 것이다.
		if (iter->matWorld == XMMatrixIdentity())
		{
			if (iter->eShape == SHAPE_TYPE::LINE)
			{
				Vec3 vStartPos = iter->vStartPos;
				Vec3 vEndPos = iter->vEndPos;
				
				Vec3 vDirection = vEndPos - vStartPos;
				float fLength = vDirection.Length();
				vDirection.Normalize();

				// Set the position of the pShapeObj to the start position
				pShapeObj->Transform()->SetRelativePos(vStartPos);

				// Set the scale of the pShapeObj
				float fThickness = 0.01f; // Adjust the thickness of the line segment as desired
				pShapeObj->Transform()->SetRelativeScale({ fLength, fThickness, 0.0f });

				// Calculate and set the rotation of the pShapeObj to align it with the direction vector
				float fRotAngle = atan2(vDirection.y, vDirection.x);
				pShapeObj->Transform()->SetRelativeRot({ 0.0f, 0.0f, fRotAngle });
			}
			else
			{
				pShapeObj->Transform()->SetRelativePos(iter->vWorldPos);
				pShapeObj->Transform()->SetRelativeScale(iter->vWorldScale);
				pShapeObj->Transform()->SetRelativeRot(iter->vWorldRot);
			}

			pShapeObj->finaltick();
		}
		else
		{
			pShapeObj->Transform()->SetWorldMat(iter->matWorld);
		}

		// 위치정보 처리 후에는 Mtrl의 상수버퍼를 통해 색상 정보를 바인딩한다
		pShapeObj->MeshRender()->GetMaterial()->SetScalarParam(VEC4_0, &iter->vColor);

		// 최종적으로 해당 DebugShapeObj를 렌더링한다. 
		pShapeObj->render();

		// CurTime을 계산하여 MaxTime이 넘을 경우 벡터에서 삭제한다.
		// MaxTime이 0이더라도 한 tick은 렌더링이 된다.
		iter->fCurTime += DT;
		if (iter->fMaxTime < iter->fCurTime)
		{
			iter = m_DebugShapeInfo.erase(iter);
		}
		else
		{
			iter++;
		}
	}
}
