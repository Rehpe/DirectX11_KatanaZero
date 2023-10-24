#pragma once
#include "CSingleton.h"

// 더 이상 레벨 매니저는 오브젝트들의 렌더링에 관여하지 않는다. 렌더링은 카메라에서 진행된다.

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

