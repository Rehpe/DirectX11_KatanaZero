#include "pch.h"
#include "CEventManager.h"

#include "CGameObject.h"
#include "CLevelManager.h"
#include "CLevel.h"
#include "CLayer.h"
#include "CPrefab.h"
#include "ptr.h"
#include "CResourceManager.h"
#include "CRenderManager.h"

CEventManager::CEventManager()
{
}

CEventManager::~CEventManager()
{
}

void CEventManager::tick()
{
	// 레벨 변경 여부 리셋
	m_LevelChanged = false;

	// 이벤트매니저의 tick 직전, GC에 들어있는 Dead 오브젝트 일괄 삭제 후 벡터 비우기
	GC_Clear();
	
	for (size_t i = 0; i < m_vecEvent.size(); i++)
	{
		switch (m_vecEvent[i].Type)
		{
		case EVENT_TYPE::CREATE_OBJECT:		// wParam : 생성할 오브젝트 주소, lParam : 추가할 레이어 번호
		{
			// 오브젝트 생성
			CGameObject* NewObj = (CGameObject*) m_vecEvent[i].wParam;
			int iLayerIdx = (int) m_vecEvent[i].lParam;

			CLevelManager::GetInst()->GetCurLevel()->AddGameObject(NewObj, iLayerIdx, false);
			
			// 오브젝트가 생성된 시점에 이미 Level이 Play모드인 경우, 바로 begin 호출
			if (CLevelManager::GetInst()->GetCurLevel()->GetState() == LEVEL_STATE::PLAY)
			{
				NewObj->begin();
			}

			m_LevelChanged = true;
		}
			break;
		case EVENT_TYPE::DELETE_OBJECT:
		{
			// 오브젝트 삭제
			CGameObject* DeleteObject = (CGameObject*)m_vecEvent[i].wParam;

			// GC에 중복 pushback을 막기 위해, Dead처리가 이미 true인 경우에는 return한다
			if (false == DeleteObject->m_bDead)
			{
				DeleteObject->m_bDead = true;
				m_vecGC.push_back(DeleteObject);
			}
		}
			break;
		case EVENT_TYPE::ADD_CHILD:
			// wParam : pDestObj, lParam : pSrcObj (Dest : 부모가 될 노드 / Src : 내가 드래그한 노드 )
		{
			CGameObject* pDestObj = (CGameObject*)m_vecEvent[i].wParam;
			CGameObject* pSrcObj = (CGameObject*)m_vecEvent[i].lParam;

			// 허공에 드래그했으면서(Dest 미지정)
			if (nullptr == pDestObj)
			{
				// Src노드가 최상위 부모가 아닐 때
				if (pSrcObj->GetParent())
				{
					// Src노드가 최상위 부모 오브젝트가 된다.
					
					// 기존 부모와의 연결 해제
					pSrcObj->DisconnectFromParent();

					// 최상위 부모 오브젝트로, 소속 레이어에 등록
					pSrcObj->AddParentList();
				}
			}
			else
			{
				pDestObj->AddChild(pSrcObj);
			}

			m_LevelChanged = true;
		}
			break;
		case EVENT_TYPE::CHANGE_LAYER:
		{
			CGameObject* Obj = (CGameObject*)m_vecEvent[i].wParam;
			int NewLayerIdx = (int)m_vecEvent[i].lParam;

			// 인자로 들어온 오브젝트가 최상위 부모 오브젝트라면
			if (Obj->GetParent() == nullptr)
			{
				// 레이어의 부모 벡터에서 해당 오브젝트 삭제
				Obj->ChangeToChildType();
				if (NewLayerIdx != -1)
				{
					CLevelManager::GetInst()->GetCurLevel()->AddGameObject(Obj, NewLayerIdx, false);
				}
			}
			else
			{
				Obj->m_iLayerIdx = NewLayerIdx;
			}
			
			m_LevelChanged = true;
		}
			break;
		case EVENT_TYPE::ADD_PREFAB:
		{
			CGameObject* Obj = (CGameObject*)m_vecEvent[i].wParam;

			// 인자로 들어온 오브젝트를 Prefab으로 등록
			Ptr<CPrefab> NewPrefab = new CPrefab;
			NewPrefab->RegisterProtoObject(Obj);
			CResourceManager::GetInst()->AddRes<CPrefab>(L"NewPrefab", NewPrefab);

			m_LevelChanged = true;
		}
			break;
		case EVENT_TYPE::DELETE_RESOURCE:
			// wParam : RES_TYPE, lParam : Resource Adress
		{
			RES_TYPE type = (RES_TYPE)m_vecEvent[i].wParam;
			CResource* pRes = (CResource*)m_vecEvent[i].lParam;
			CResourceManager::GetInst()->DeleteRes(type, pRes->GetKey());
		}
			break;
		case EVENT_TYPE::LEVEL_CHANGE:
		{
			// wParam : Level Adress
			CLevel* Level = (CLevel*)m_vecEvent[i].wParam;
			CRenderManager::GetInst()->ClearCamera();
			CLevelManager::GetInst()->ChangeLevel(Level);
			m_LevelChanged = true;

			Level->ChangeState(LEVEL_STATE::PLAY);
		}
			break;
		}
	}

	// 벡터 내의 모든 이벤트를 처리했다면 이벤트 벡터를 비운다.
	m_vecEvent.clear();
}

void CEventManager::GC_Clear()
{
	for (size_t i = 0; i < m_vecGC.size(); ++i)
	{
		if (nullptr != m_vecGC[i])
		{
			// 자식 타입 오브젝트인 경우, 부모와의 관계를 끊어준 뒤 삭제한다.
			if (m_vecGC[i]->GetParent())
				m_vecGC[i]->DisconnectFromParent();

			delete m_vecGC[i];
			m_vecGC[i] = nullptr;

			m_LevelChanged = true;
		}
	}
	m_vecGC.clear();
}
