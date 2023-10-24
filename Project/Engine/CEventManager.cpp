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
	// ���� ���� ���� ����
	m_LevelChanged = false;

	// �̺�Ʈ�Ŵ����� tick ����, GC�� ����ִ� Dead ������Ʈ �ϰ� ���� �� ���� ����
	GC_Clear();
	
	for (size_t i = 0; i < m_vecEvent.size(); i++)
	{
		switch (m_vecEvent[i].Type)
		{
		case EVENT_TYPE::CREATE_OBJECT:		// wParam : ������ ������Ʈ �ּ�, lParam : �߰��� ���̾� ��ȣ
		{
			// ������Ʈ ����
			CGameObject* NewObj = (CGameObject*) m_vecEvent[i].wParam;
			int iLayerIdx = (int) m_vecEvent[i].lParam;

			CLevelManager::GetInst()->GetCurLevel()->AddGameObject(NewObj, iLayerIdx, false);
			
			// ������Ʈ�� ������ ������ �̹� Level�� Play����� ���, �ٷ� begin ȣ��
			if (CLevelManager::GetInst()->GetCurLevel()->GetState() == LEVEL_STATE::PLAY)
			{
				NewObj->begin();
			}

			m_LevelChanged = true;
		}
			break;
		case EVENT_TYPE::DELETE_OBJECT:
		{
			// ������Ʈ ����
			CGameObject* DeleteObject = (CGameObject*)m_vecEvent[i].wParam;

			// GC�� �ߺ� pushback�� ���� ����, Deadó���� �̹� true�� ��쿡�� return�Ѵ�
			if (false == DeleteObject->m_bDead)
			{
				DeleteObject->m_bDead = true;
				m_vecGC.push_back(DeleteObject);
			}
		}
			break;
		case EVENT_TYPE::ADD_CHILD:
			// wParam : pDestObj, lParam : pSrcObj (Dest : �θ� �� ��� / Src : ���� �巡���� ��� )
		{
			CGameObject* pDestObj = (CGameObject*)m_vecEvent[i].wParam;
			CGameObject* pSrcObj = (CGameObject*)m_vecEvent[i].lParam;

			// ����� �巡�������鼭(Dest ������)
			if (nullptr == pDestObj)
			{
				// Src��尡 �ֻ��� �θ� �ƴ� ��
				if (pSrcObj->GetParent())
				{
					// Src��尡 �ֻ��� �θ� ������Ʈ�� �ȴ�.
					
					// ���� �θ���� ���� ����
					pSrcObj->DisconnectFromParent();

					// �ֻ��� �θ� ������Ʈ��, �Ҽ� ���̾ ���
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

			// ���ڷ� ���� ������Ʈ�� �ֻ��� �θ� ������Ʈ���
			if (Obj->GetParent() == nullptr)
			{
				// ���̾��� �θ� ���Ϳ��� �ش� ������Ʈ ����
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

			// ���ڷ� ���� ������Ʈ�� Prefab���� ���
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

	// ���� ���� ��� �̺�Ʈ�� ó���ߴٸ� �̺�Ʈ ���͸� ����.
	m_vecEvent.clear();
}

void CEventManager::GC_Clear()
{
	for (size_t i = 0; i < m_vecGC.size(); ++i)
	{
		if (nullptr != m_vecGC[i])
		{
			// �ڽ� Ÿ�� ������Ʈ�� ���, �θ���� ���踦 ������ �� �����Ѵ�.
			if (m_vecGC[i]->GetParent())
				m_vecGC[i]->DisconnectFromParent();

			delete m_vecGC[i];
			m_vecGC[i] = nullptr;

			m_LevelChanged = true;
		}
	}
	m_vecGC.clear();
}
