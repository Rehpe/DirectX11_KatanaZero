#pragma once
#include "CEntity.h"

class CLayer;
class CGameObject;

class CLevel :
    public CEntity
{
public:
    CLevel();
    ~CLevel();

    friend class CLevelManager;

public:
    CLONE_DEACTIVATE(CLevel);

private:
    CLayer*         m_arrLayer[MAX_LAYER];
    LEVEL_STATE     m_State;

public:
    void begin();
    void tick();
    void finaltick();

public:
    CLayer* FindLayerByName(const wstring& _strName);
    void AddGameObject(CGameObject* _Object, int _iLayerIdx, bool _bMove);
    void AddGameObject(CGameObject* _Object, const wstring& _LayerName, bool _Move);

    CLayer* GetLayer(int _iLayerIdx) { assert(!(_iLayerIdx < 0)); return m_arrLayer[_iLayerIdx]; }

    CGameObject* FindObjectByName(const wstring& _Name);
    void FindObjectByName(const wstring& _Name, vector<CGameObject*>& _Out);

    CGameObject* FindObjectByID(const int& _ID);

    void ChangeState(LEVEL_STATE _State);
    LEVEL_STATE GetState() { return m_State; }

private:
    // 레이어의 '모든 오브젝트 벡터' 에 등록된 GameObject 일괄 제거
    void clear();

};

