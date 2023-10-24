#pragma once
#include "UI.h"

class ListUI :
    public UI
{
public:
    ListUI();
    ~ListUI();

private:
    vector<string>          m_vecStrData;
    int                     m_iSelectedIdx;     // 선택된 인덱스 값

    UI*                     m_SelectInst;       // 객체
    UI_DELEGATE_1           m_SelectDelegate;   // 함수(반환타입 void, 인자 하나)

public:
    virtual void finaltick() override;
    virtual int render_update() override;

public:
    // List에 원하는 항목 추가
    void AddItem(const string& _strItem) { m_vecStrData.push_back(_strItem); }
    void Clear() { m_vecStrData.clear(); m_iSelectedIdx = -1; }

    void Reset(const string& _strName, ImVec2 _vSize);

    // 델리게이트 등록 함수
    void AddDynamic_Select(UI* _Inst, UI_DELEGATE_1 _Func)
    {
        m_SelectInst = _Inst;
        m_SelectDelegate = _Func;
    }
};

