#include "pch.h"
#include "CLevel.h"

#include "CLayer.h"
#include "CGameObject.h"
#include "CRenderManager.h"

CLevel::CLevel()
	: m_arrLayer{}
	, m_State(LEVEL_STATE::STOP) // ������ �⺻ ���´� Stop
{
	for (UINT i = 0; i < MAX_LAYER; i++)
	{
		// ������ �����ڿ��� 32���� ���̾ �ϰ������� �����ϸ�, �̶� �ڽ�(���̾�)�� �ε��� ��ȣ�� �����Ѵ�.
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
	// ��ü 32�� ���̾ ���� ���ڷ� ���� �̸��� ���� ���̾ ã�� ���̾��ȣ�� ��ȯ
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
		// �ش� ������ ��� ������Ʈ�� �����ͼ� �̸����� �˻��Ѵ�.
		const vector<CGameObject*>& vecObjects = m_arrLayer[i]->GetPreObjects();
		for (size_t j = 0; j < vecObjects.size(); ++j)
		{
			// ���� ���� ã���� ������Ʈ �� ���� ��ȯ�Ѵ�.
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
		// �ش� ������ ��� ������Ʈ�� �����ͼ� �̸����� �˻��Ѵ�.
		const vector<CGameObject*>& vecObjects = m_arrLayer[i]->GetPreObjects();
		for (size_t j = 0; j < vecObjects.size(); ++j)
		{
			// �˻��Ǵ� ��� ������Ʈ���� �Է� ���Ϳ� �־� ��ȯ�Ѵ�.
			if (_Name == vecObjects[j]->GetName())
				_Out.push_back(vecObjects[j]);
		}
	}
}

CGameObject* CLevel::FindObjectByID(const int& _ID)
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		// �ش� ������ ��� ������Ʈ�� �����ͼ� ID�� �˻��Ѵ�.
		const vector<CGameObject*>& vecObjects = m_arrLayer[i]->GetObjects();
		for (size_t j = 0; j < vecObjects.size(); ++j)
		{
			// ���� ���� ã���� ������Ʈ �� ���� ��ȯ�Ѵ�.
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

		// ����� �� PreObj ���ͷ� ��� ������ �ű��.
		m_arrLayer[i]->m_vecPreObject = move(m_arrLayer[i]->m_vecObject);

		m_arrLayer[i]->m_vecObject.clear();
	}
}
