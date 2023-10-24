#include "pch.h"
#include "CLevel.h"

#include "CLayer.h"
#include "CGameObject.h"
#include "CRenderManager.h"

CLevel::CLevel()
	: m_arrLayer{}
	, m_State(LEVEL_STATE::STOP) // 레벨의 기본 상태는 Stop
{
	for (UINT i = 0; i < MAX_LAYER; i++)
	{
		// 레벨의 생성자에서 32개의 레이어를 일괄적으로 생성하며, 이때 자신(레이어)의 인덱스 번호를 저장한다.
		m_arrLayer[i] = new CLayer;
		m_arrLayer[i]->m_iLayerIdx = i;
	}

}

CLevel::~CLevel()
{
	Safe_Del_Array(m_arrLayer);
}

void CLevel::begin()
{
	for (UINT i = 0; i < MAX_LAYER; i++)
	{
		m_arrLayer[i]->begin();
	}
}

void CLevel::tick()
{
	for (UINT i = 0; i < MAX_LAYER; i++)
	{
		m_arrLayer[i]->tick();
	}
}

void CLevel::finaltick()
{
	for (UINT i = 0; i < MAX_LAYER; i++)
	{
		m_arrLayer[i]->finaltick();
	}
}

CLayer* CLevel::FindLayerByName(const wstring& _strName)
{
	// 전체 32개 레이어를 돌며 인자로 들어온 이름을 가진 레이어를 찾아 레이어번호를 반환
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		if (m_arrLayer[i]->GetName() == _strName)
			return m_arrLayer[i];
	}

	return nullptr;
}

void CLevel::AddGameObject(CGameObject* _Object, int _iLyaerIdx, bool _bMove)
{
	m_arrLayer[_iLyaerIdx]->AddGameObject(_Object,_bMove);
}

void CLevel::AddGameObject(CGameObject* _Object, const wstring& _LayerName, bool _Move)
{
	CLayer* pLayer = FindLayerByName(_LayerName);
	assert(pLayer);

	pLayer->AddGameObject(_Object, _Move);
}

CGameObject* CLevel::FindObjectByName(const wstring& _Name)
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		// 해당 레벨의 모든 오브젝트를 가져와서 이름으로 검색한다.
		const vector<CGameObject*>& vecObjects = m_arrLayer[i]->GetPreObjects();
		for (size_t j = 0; j < vecObjects.size(); ++j)
		{
			// 가장 먼저 찾아진 오브젝트 한 개를 반환한다.
			if (_Name == vecObjects[j]->GetName())
				return vecObjects[j];
		}
	}
	return nullptr;
}

void CLevel::FindObjectByName(const wstring& _Name, vector<CGameObject*>& _Out)
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		// 해당 레벨의 모든 오브젝트를 가져와서 이름으로 검색한다.
		const vector<CGameObject*>& vecObjects = m_arrLayer[i]->GetPreObjects();
		for (size_t j = 0; j < vecObjects.size(); ++j)
		{
			// 검색되는 모든 오브젝트들을 입력 벡터에 넣어 반환한다.
			if (_Name == vecObjects[j]->GetName())
				_Out.push_back(vecObjects[j]);
		}
	}
}

CGameObject* CLevel::FindObjectByID(const int& _ID)
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		// 해당 레벨의 모든 오브젝트를 가져와서 ID로 검색한다.
		const vector<CGameObject*>& vecObjects = m_arrLayer[i]->GetObjects();
		for (size_t j = 0; j < vecObjects.size(); ++j)
		{
			// 가장 먼저 찾아진 오브젝트 한 개를 반환한다.
			if (_ID == vecObjects[j]->GetID())
				return vecObjects[j];
		}
	}
	return nullptr;
}

void CLevel::ChangeState(LEVEL_STATE _State)
{
	m_State = _State;

	if (LEVEL_STATE::PLAY == m_State)
	{
		CRenderManager::GetInst()->SetRenderFunc(true);
		begin();
	}
	else
	{
		CRenderManager::GetInst()->SetRenderFunc(false);
	}
}

void CLevel::clear()
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		if(!m_arrLayer[i]->m_vecPreObject.empty())
			m_arrLayer[i]->m_vecPreObject.clear();

		// 지우기 전 PreObj 벡터로 모든 정보를 옮긴다.
		m_arrLayer[i]->m_vecPreObject = move(m_arrLayer[i]->m_vecObject);

		m_arrLayer[i]->m_vecObject.clear();
	}
}
