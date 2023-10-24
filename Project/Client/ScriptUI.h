#pragma once
#include "ComponentUI.h"

#include <Engine\CScript.h>

class ScriptUI :
    public ComponentUI
{

public:
    ScriptUI();
    ~ScriptUI();

private:
    CScript*        m_pTargetScript;        // � ��ũ��Ʈ�� ǥ���� ������
    string          m_strScriptName;        // ǥ���� �ش� ��ũ��Ʈ�� �̸�

public:
    virtual int render_update() override;

public:
    void SetScript(CScript* _Script);
};

