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
    int                     m_iSelectedIdx;     // ���õ� �ε��� ��

    UI*                     m_SelectInst;       // ��ü
    UI_DELEGATE_1           m_SelectDelegate;   // �Լ�(��ȯŸ�� void, ���� �ϳ�)

public:
    virtual void finaltick() override;
    virtual int render_update() override;

public:
    // List�� ���ϴ� �׸� �߰�
    void AddItem(const string& _strItem) { m_vecStrData.push_back(_strItem); }
    void Clear() { m_vecStrData.clear(); m_iSelectedIdx = -1; }

    void Reset(const string& _strName, ImVec2 _vSize);

    // ��������Ʈ ��� �Լ�
    void AddDynamic_Select(UI* _Inst, UI_DELEGATE_1 _Func)
    {
        m_SelectInst = _Inst;
        m_SelectDelegate = _Func;
    }
};

