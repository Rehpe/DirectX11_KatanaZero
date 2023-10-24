#include "pch.h"
#include "Test.h"

#include "ptr.h"
#include "CDevice.h"
#include "CPathManager.h"
#include "CKeyManager.h"
#include "CTimeManager.h"
#include "CResourceManager.h"

#include "CGameObject.h"
#include "CComponent.h"
#include "CTransform.h"
#include "CMeshRender.h"
#include "CMesh.h"
#include "CGraphicsShader.h"
#include "CPlayerScript.h"

// GameObject 
CGameObject* g_Obj = nullptr;
 
void Init()
{
	// 오브젝트 생성
	g_Obj = new CGameObject;
	g_Obj->AddComponent(new CTransform);
	g_Obj->AddComponent(new CMeshRender);
	g_Obj->AddComponent(new CPlayerScript);
	
	Ptr<CMesh> RectMesh = CResourceManager::GetInst()->FindRes<CMesh>(L"RectMesh");
	Ptr<CGraphicsShader> TestShader = CResourceManager::GetInst()->FindRes<CGraphicsShader>(L"TestShader");

	g_Obj->MeshRender()->SetMesh(RectMesh);
	g_Obj->MeshRender()->SetShader(TestShader);
}

void Tick()
{
	g_Obj->tick();
	g_Obj->finaltick();
}

void Render()
{
	
	g_Obj->render();
}

void Release()
{
	delete g_Obj;
}
