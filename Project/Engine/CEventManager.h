#pragma once
#include "CSingleton.h"

// ���� ������Ʈ�� ����, ����, ���� ��ȯ ���� �߿� �̺�Ʈ�� ���� ���������� �̷��ִ� ����

class CEventManager :
    public CSingleton<CEventManager>
{
    SINGLETON(CEventManager);

private:
    vector<tEvent>          m_vecEvent;
    vector<CGameObject*>    m_vecGC;        // Dead ó���� ������Ʈ���� �־�ΰ�, ���� �����ӿ��� �ϰ� ����

    bool                    m_LevelChanged; // ���� ������Ʈ�� �߰�/���� ���� �������� �ִ���

public:
    void tick();

private:
    void GC_Clear();

public:
    void AddEvent(const tEvent& _event) { m_vecEvent.push_back(_event); }
    bool IsLevelChanged() { return m_LevelChanged; }


};

