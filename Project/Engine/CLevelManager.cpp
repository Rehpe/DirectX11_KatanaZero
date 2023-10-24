#include "pch.h"
#include "CLevelManager.h"

#include "CLevel.h"
#include "CLayer.h"
#include "CResourceManager.h"
#include "CCollisionManager.h"

#include "CGameObject.h"
#include "CScript.h"
#include "component.h"
#include "CParticleSystem.h"
#include "CSetColorShader.h"

#include "CStructuredBuffer.h"

CLevelManager::CLevelManager()
	: m_pCurLevel(nullptr)
	, m_pGameManager(nullptr)
	, m_GameManager(nullptr)
{
}

CLevelManager::~CLevelManager()
{
	if (nullptr != m_pCurLevel)
		delete m_pCurLevel;
}

void CLevelManager::init()
{
	m_pCurLevel = new CLevel;
	
}
class CGameManagerScript;
void CLevelManager::tick()
{
	m_pCurLevel->clear();

	// ������ Play ������ ��쿡�� tick�� ȣ���Ѵ�
	if (LEVEL_STATE::PLAY == m_pCurLevel->GetState())
	{
		m_pCurLevel->tick();
	}
	else if (m_pGameManager)
	{
		// GameManagerScript�� ��� ���� ���¿� ��� ���� �� tick�� �޴´�.
		m_pGameManager->tick(); 
	}

	// finaltick�� ��� ������ ���ε��� ����ϹǷ� ������ ȣ���Ѵ�.
	m_pCurLevel->finaltick();
}

CGameObject* CLevelManager::FindObjectByName(const wstring& _strName)
{
	return m_pCurLevel->FindObjectByName(_strName);	
}

void CLevelManager::FindObjectByName(const wstring& _strName, vector<CGameObject*>& _vec)
{
	m_pCurLevel->FindObjectByName(_strName, _vec);
}

CGameObject* CLevelManager::FindObjectByID(const int& _ID)
{
	return m_pCurLevel->FindObjectByID(_ID);
}

void CLevelManager::ChangeLevel(CLevel* _NextLevel)
{
	if (nullptr != m_pCurLevel)
	{
		delete m_pCurLevel;
		m_pCurLevel = nullptr;
	}
	m_pCurLevel = _NextLevel;

	//_NextLevel->ChangeState(LEVEL_STATE::PLAY);
	//if (m_GameManager)
		//_NextLevel->AddGameObject(m_GameManager, 0, false); //(m_GameManager, Vec3(0.f, 0.f, 0.f), 0);
}
