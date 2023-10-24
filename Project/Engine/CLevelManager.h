#pragma once
#include "CSingleton.h"

// �� �̻� ���� �Ŵ����� ������Ʈ���� �������� �������� �ʴ´�. �������� ī�޶󿡼� ����ȴ�.

class CLevel;
class CScript;
class CGameObject;

class CLevelManager :
    public CSingleton<CLevelManager>
{
    SINGLETON(CLevelManager);

private:
    CLevel*              m_pCurLevel;
    CScript*             m_pGameManager;
    CGameObject*         m_GameManager;

public:
    void init();
    void tick();

public:
    void RegisterGameManagerScript(CScript* _GM) { m_pGameManager = _GM; };
    void RegisterGameManager(CGameObject* _GMObj) { m_GameManager = _GMObj; }

    CLevel* GetCurLevel() { return m_pCurLevel; }
    CGameObject* FindObjectByName(const wstring& _strName);
    void FindObjectByName(const wstring& _strName, vector<CGameObject*>& _Out);
    CGameObject* FindObjectByID(const int& _ID);

    void ChangeLevel(CLevel* _NextLevel);

};

