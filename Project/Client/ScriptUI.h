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
    CScript*        m_pTargetScript;        // 어떤 스크립트를 표시할 것인지
    string          m_strScriptName;        // 표시할 해당 스크립트의 이름

public:
    virtual int render_update() override;

public:
    void SetScript(CScript* _Script);
};

