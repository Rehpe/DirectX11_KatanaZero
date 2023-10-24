#include "pch.h"
#include "FSMUI.h"

#include <Engine\CGameObject.h>
#include <Engine\CFSM.h>
#include <Engine\CState.h>

FSMUI::FSMUI()
    : ComponentUI("##FSM", COMPONENT_TYPE::FSM_AI)
{
    SetName("FSM");
}

FSMUI::~FSMUI()
{
}

void FSMUI::init()
{
}

int FSMUI::render_update()
{
    if (FALSE == ComponentUI::render_update())
        return FALSE;

    // 현재 상태 출력

    CState* pCurState = GetTarget()->FSM()->GetCurState();
    char szBuff[50] = {};
    if (pCurState != nullptr)
    {
        WstringToChar(pCurState->GetName(), szBuff, 50);
    }
   
    ImGui::Text("Current State : ");
    ImGui::SameLine();
    ImGui::InputText("##StateName", szBuff, 50, ImGuiInputTextFlags_ReadOnly);

    return 0;
}
