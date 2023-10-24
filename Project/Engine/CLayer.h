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
    vector<CGameObject*>    m_vecParentObj; // ���꿡�� ����ϴ�, ���̾ ���� �ֻ��� �θ� ������Ʈ
    vector<CGameObject*>    m_vecObject;    // ���� ������ ��� ���� �ش� ���̾ �����ִ� ��� ������Ʈ

    vector<CGameObject*>    m_vecPreObject; // ��ϵ� ������Ʈ���� �������� �� ���� ����

    int                     m_iLayerIdx;    // �ڽ��� ���̾� ��ȣ (0~31)

public:
    void begin();
    void tick();
    void finaltick();

public:
    // �ֻ��� �θ� ������Ʈ���� �ش� �Լ��� ȣ���ؾ��ϸ�, �ڽ� ����� �� ��ģ ���¿��� ���������� ȣ���ؾ���.
    void AddGameObject(CGameObject* _Object, bool _bMove);      

    const vector<CGameObject*>& GetObjects() { return m_vecObject; }
    const vector<CGameObject*>& GetPreObjects() { return m_vecPreObject; }
    const vector<CGameObject*>& GetParentObject() { return m_vecParentObj; }

    int GetLayerIndex() { return m_iLayerIdx; }

    // �θ� ������Ʈ ���Ͱ� �ƴ�, ��� ���Ϳ� ����ϴ� �Լ�
    void RegisterObject(CGameObject* _Object) { m_vecObject.push_back(_Object); }

    void RemoveFromParentList(CGameObject* _Obj);
    void AddParentList(CGameObject* _Obj);
};

