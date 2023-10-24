#include "pch.h"
#include "ListUI.h"

#include <Engine\CKeyManager.h>

ListUI::ListUI()
    : UI("##List")
    , m_iSelectedIdx(0)
{
}

ListUI::~ListUI()
{
}

void ListUI::finaltick()
{
    // 부모의 finaltick 먼저 호출
    UI::finaltick();

    // Esc 눌리면 비활성화
    if (KEY_TAP(KEY::ESC))
    {
        SetActive(false);

        // 모든 UI 포커스 해제
        ImGui::SetWindowFocus(nullptr);
    }
}

int ListUI::render_update()
{
    // 최근 UI 의 작업영역 사이즈를 알아낸다. (타이틀바 제외한 실제 작업영역)
    ImVec2 ListUIContentSize = ImGui::GetContentRegionAvail();

    if (ImGui::BeginListBox("##list", ListUIContentSize))   // Id, Listbox 사이즈
    {
        // 저장한 문자열 벡터의 크기만큼 반복문을 돌며 리스트를 출력한다
        for (int i = 0; i < m_vecStrData.size(); i++)
        {
            const bool is_selected = (m_iSelectedIdx == i);

            // 리스트에 아이템 출력, 반환값은 클릭유무에 따라 True/ False
            if (ImGui::Selectable(m_vecStrData[i].c_str(), is_selected))
            {
                m_iSelectedIdx = i;
            }

            // 특정 아이템(인덱스 i) 위에서(Hovered) && 마우스 왼쪽 더블클릭이 발생하면(IsMouseDoubleClicked)
            if (ImGui::IsItemHovered() &&
                ImGui::IsMouseDoubleClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
            {
                // 리스트 창이 알아서 꺼진다
                SetActive(false);
                ImGui::SetWindowFocus(nullptr);

                // 실제로 발생해야하는 일(Select Delegate)이 등록되어있다면 호출해 준다.
                if (m_SelectInst && m_SelectDelegate)
                {
                    // 선택한 아이템의 key값(m_vecStrData[i].c_str())을 DWORD_PTR로 변환해서 인자로 넘겨주고 Delegate 함수 호출
                    (m_SelectInst->*m_SelectDelegate)((DWORD_PTR)m_vecStrData[i].c_str());
                }
            }

            // 클릭 시 타겟팅 된 쪽으로 Scroll Focus 기능
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }

    return TRUE;
}

void ListUI::Reset(const string& _strName, ImVec2 _vSize)
{
    // 이전 내용 삭제 초기화
    Clear();

    // List 이름, 사이즈, 활성화 여부 설정
    SetName(_strName);
    SetSize(_vSize.x, _vSize.y);
    SetActive(true);

    // 위치 조정
    HWND hWnd = ImGuiManager::GetInst()->GetMainHwnd();

    RECT rt = {};
    //GetClientRect(hWnd, &rt);         // Client : 윈도우만을 기준으로 좌상단을 0, 0으로 놓고 해상도만큼 
    GetWindowRect(hWnd, &rt);     // Window: 화면 전체를 기준으로 윈도우의 LT와 좌우 패딩을 뺀 순수 창 크기

    // 화면 중앙 좌표를 구한다.
    float CenterX = (float)((rt.left + rt.right) / 2);
    float CenterY = (float)((rt.bottom + rt.top) / 2);

    // 찾은 중앙좌표가 Popup창의 중앙이 될 수 있게 가로,세로 길이의 절반을 빼준다.
    SetPopupPos(ImVec2(CenterX - _vSize.x / 2.f, CenterY - _vSize.y / 2.f));
}
