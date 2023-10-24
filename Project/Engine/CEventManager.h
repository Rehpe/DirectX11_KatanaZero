#pragma once
#include "CSingleton.h"

// 게임 오브젝트의 생성, 삭제, 레벨 변환 등의 중요 이벤트를 다음 프레임으로 미뤄주는 역할

class CEventManager :
    public CSingleton<CEventManager>
{
    SINGLETON(CEventManager);

private:
    vector<tEvent>          m_vecEvent;
    vector<CGameObject*>    m_vecGC;        // Dead 처리된 오브젝트들을 넣어두고, 다음 프레임에서 일괄 삭제

    bool                    m_LevelChanged; // 게임 오브젝트가 추가/삭제 등의 변경점이 있는지

public:
    void tick();

private:
    void GC_Clear();

public:
    void AddEvent(const tEvent& _event) { m_vecEvent.push_back(_event); }
    bool IsLevelChanged() { return m_LevelChanged; }


};

