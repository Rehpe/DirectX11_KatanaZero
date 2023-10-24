#include "pch.h"
#include "TestLevel.h"

#include <Engine\CLevelManager.h>
#include <Engine\CLevel.h>
#include <Engine\CLayer.h>
#include <Engine\CGameObject.h>
#include <Engine\component.h>
#include <Engine\CAnim2D.h>

#include <Engine\CResourceManager.h>
#include <Engine\CCollisionManager.h>
#include <Engine\CEventManager.h>
#include <Script\CPlayerScript.h>
#include <Script\CMonsterScript.h>
#include <Script\CLevelSaveLoad.h>

#include "InspectorUI.h"

void CreateTestLevel()
{
	 
}

void LoadLevel()
{
	InitLayer();
	CollisionCheck();

	//Level 불러오기
	CLevel* pLoadedLevel = CLevelSaveLoad::LoadLevel(L"level\\Title.lv");
	tEvent evn = {};
	evn.Type = EVENT_TYPE::LEVEL_CHANGE;
	evn.wParam = (DWORD_PTR)pLoadedLevel;
	CEventManager::GetInst()->AddEvent(evn);
}


void InitLayer()
{
	// Layer 이름설정
	CLevel* pCurLevel = CLevelManager::GetInst()->GetCurLevel();

	pCurLevel->GetLayer(0)->SetName(L"Default");
	pCurLevel->GetLayer(1)->SetName(L"Ground");
	pCurLevel->GetLayer(2)->SetName(L"Wall");
	pCurLevel->GetLayer(3)->SetName(L"Door");
	pCurLevel->GetLayer(4)->SetName(L"Monster");
	pCurLevel->GetLayer(5)->SetName(L"MonsterDetector");
	pCurLevel->GetLayer(6)->SetName(L"MonsterProjectile");
	pCurLevel->GetLayer(7)->SetName(L"Switch");
	pCurLevel->GetLayer(8)->SetName(L"Effect");
	pCurLevel->GetLayer(9)->SetName(L"Player");
	pCurLevel->GetLayer(10)->SetName(L"PlayerProjectile");
	pCurLevel->GetLayer(11)->SetName(L"Ramp");
	pCurLevel->GetLayer(12)->SetName(L"Laser");
	pCurLevel->GetLayer(13)->SetName(L"Item");
	pCurLevel->GetLayer(14)->SetName(L"LaserAware");
	pCurLevel->GetLayer(15)->SetName(L"FanBlade");

	pCurLevel->GetLayer(31)->SetName(L"UI");


}

void CollisionCheck()
{
	CCollisionManager::GetInst()->LayerCheck(L"Ground", L"Player");
	CCollisionManager::GetInst()->LayerCheck(L"Ground", L"Monster");
	CCollisionManager::GetInst()->LayerCheck(L"Ground", L"Item");
	CCollisionManager::GetInst()->LayerCheck(L"Wall", L"Player");
	CCollisionManager::GetInst()->LayerCheck(L"Wall", L"Monster");
	CCollisionManager::GetInst()->LayerCheck(L"Wall", L"Item");
	CCollisionManager::GetInst()->LayerCheck(L"Door", L"Player");
	CCollisionManager::GetInst()->LayerCheck(L"Door", L"Monster");
	CCollisionManager::GetInst()->LayerCheck(L"Door", L"Item");
	CCollisionManager::GetInst()->LayerCheck(L"Monster", L"Player");
	CCollisionManager::GetInst()->LayerCheck(L"Monster", L"PlayerProjectile");
	CCollisionManager::GetInst()->LayerCheck(L"Monster", L"Item");
	CCollisionManager::GetInst()->LayerCheck(L"MonsterDetector", L"Player");
	CCollisionManager::GetInst()->LayerCheck(L"MonsterDetector", L"Door");
	CCollisionManager::GetInst()->LayerCheck(L"MonsterProjectile", L"Player");
	CCollisionManager::GetInst()->LayerCheck(L"MonsterProjectile", L"PlayerProjectile");
	CCollisionManager::GetInst()->LayerCheck(L"Player", L"Item");
	CCollisionManager::GetInst()->LayerCheck(L"PlayerProjectile", L"Ground");
	CCollisionManager::GetInst()->LayerCheck(L"Player", L"Ramp");
	CCollisionManager::GetInst()->LayerCheck(L"Monster", L"Ramp");
	CCollisionManager::GetInst()->LayerCheck(L"Laser", L"Player");
	CCollisionManager::GetInst()->LayerCheck(L"Laser", L"Monster");
	CCollisionManager::GetInst()->LayerCheck(L"Switch", L"Player");
	CCollisionManager::GetInst()->LayerCheck(L"FanBlade", L"Player");
	CCollisionManager::GetInst()->LayerCheck(L"LaserAware", L"Monster");

}
