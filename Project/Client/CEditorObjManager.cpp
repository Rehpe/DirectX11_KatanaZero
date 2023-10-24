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
	// ����� ������ ����
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

	// Editor Camera ����
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
	// tick-render ��, RenderManager���Լ� DebugShape ���� ��������
	vector<tDebugShapeInfo>& vecInfo = CRenderManager::GetInst()->GetDebugShapeInfo();
	
	// �޾ƿ� DebugShape ������ ����� m_DebugShapeInfo�� �״�� ����
	m_DebugShapeInfo.insert(m_DebugShapeInfo.end(), vecInfo.begin(), vecInfo.end());
	
	// �޾ƿ� �������� ���ʹ� clear
	vecInfo.clear();

	tick();
	render();
}

void CEditorObjManager::tick()
{
	// Editor Camera�� ������ EditorObject�� EditorObjectManager�� tick, render �� �����ش�. 
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
	// Editor Camera�� ������ EditorObject�� EditorObjectManager�� tick, render �� �����ش�. 
	for (size_t i = 0; i < m_vecEditorObj.size(); ++i)
	{
		m_vecEditorObj[i]->render();
	}

	// DebugShape Render
	CGameObjectEX* pShapeObj = nullptr;

	// ���ӽð��� ����� Obj�� ���Ϳ��� �����ϹǷ� iter�� ����Ѵ�.
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

		// DebugShape�� ����� ��������̶�� -> ����� �ƴ� ���� ��ġ,ũ��,ȸ�� 3���� ���� ���Դٴ� �Ҹ�
		// ���� ��ġ,ũ��,ȸ������ SetRelative�Լ��� ���� �� finaltick�� ���� ����� ���Ǿ� ���� ���̴�.
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

		// ��ġ���� ó�� �Ŀ��� Mtrl�� ������۸� ���� ���� ������ ���ε��Ѵ�
		pShapeObj->MeshRender()->GetMaterial()->SetScalarParam(VEC4_0, &iter->vColor);

		// ���������� �ش� DebugShapeObj�� �������Ѵ�. 
		pShapeObj->render();

		// CurTime�� ����Ͽ� MaxTime�� ���� ��� ���Ϳ��� �����Ѵ�.
		// MaxTime�� 0�̴��� �� tick�� �������� �ȴ�.
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
