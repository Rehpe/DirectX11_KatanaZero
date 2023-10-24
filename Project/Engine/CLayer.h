#pragma once
#include "CEntity.h"

class CGameObject;

class CLayer :
    public CEntity
{
public:
    CLayer();
    ~CLayer();

    friend class CLevel;
    friend class CGameObject;
    friend class CEventManager;
    
public:
    CLONE(CLayer);
    
private:
    vector<CGameObject*>    m_vecParentObj; // 연산에서 사용하는, 레이어에 속한 최상위 부모 오브젝트
    vector<CGameObject*>    m_vecObject;    // 계층 구조에 상관 없이 해당 레이어에 속해있는 모든 오브젝트

    vector<CGameObject*>    m_vecPreObject; // 등록된 오브젝트들이 지워지기 전 담기는 벡터

    int                     m_iLayerIdx;    // 자신의 레이어 번호 (0~31)

public:
    void begin();
    void tick();
    void finaltick();

public:
    // 최상위 부모 오브젝트에만 해당 함수를 호출해야하며, 자식 등록을 다 마친 상태에서 마지막으로 호출해야함.
    void AddGameObject(CGameObject* _Object, bool _bMove);      

    const vector<CGameObject*>& GetObjects() { return m_vecObject; }
    const vector<CGameObject*>& GetPreObjects() { return m_vecPreObject; }
    const vector<CGameObject*>& GetParentObject() { return m_vecParentObj; }

    int GetLayerIndex() { return m_iLayerIdx; }

    // 부모 오브젝트 벡터가 아닌, 모든 벡터에 등록하는 함수
    void RegisterObject(CGameObject* _Object) { m_vecObject.push_back(_Object); }

    void RemoveFromParentList(CGameObject* _Obj);
    void AddParentList(CGameObject* _Obj);
};

