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
    // �θ��� finaltick ���� ȣ��
    UI::finaltick();

    // Esc ������ ��Ȱ��ȭ
    if (KEY_TAP(KEY::ESC))
    {
        SetActive(false);

        // ��� UI ��Ŀ�� ����
        ImGui::SetWindowFocus(nullptr);
    }
}

int ListUI::render_update()
{
    // �ֱ� UI �� �۾����� ����� �˾Ƴ���. (Ÿ��Ʋ�� ������ ���� �۾�����)
    ImVec2 ListUIContentSize = ImGui::GetContentRegionAvail();

    if (ImGui::BeginListBox("##list", ListUIContentSize))   // Id, Listbox ������
    {
        // ������ ���ڿ� ������ ũ�⸸ŭ �ݺ����� ���� ����Ʈ�� ����Ѵ�
        for (int i = 0; i < m_vecStrData.size(); i++)
        {
            const bool is_selected = (m_iSelectedIdx == i);

            // ����Ʈ�� ������ ���, ��ȯ���� Ŭ�������� ���� True/ False
            if (ImGui::Selectable(m_vecStrData[i].c_str(), is_selected))
            {
                m_iSelectedIdx = i;
            }

            // Ư�� ������(�ε��� i) ������(Hovered) && ���콺 ���� ����Ŭ���� �߻��ϸ�(IsMouseDoubleClicked)
            if (ImGui::IsItemHovered() &&
                ImGui::IsMouseDoubleClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
            {
                // ����Ʈ â�� �˾Ƽ� ������
                SetActive(false);
                ImGui::SetWindowFocus(nullptr);

                // ������ �߻��ؾ��ϴ� ��(Select Delegate)�� ��ϵǾ��ִٸ� ȣ���� �ش�.
                if (m_SelectInst && m_SelectDelegate)
                {
                    // ������ �������� key��(m_vecStrData[i].c_str())�� DWORD_PTR�� ��ȯ�ؼ� ���ڷ� �Ѱ��ְ� Delegate �Լ� ȣ��
                    (m_SelectInst->*m_SelectDelegate)((DWORD_PTR)m_vecStrData[i].c_str());
                }
            }

            // Ŭ�� �� Ÿ���� �� ������ Scroll Focus ���
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }

    return TRUE;
}

void ListUI::Reset(const string& _strName, ImVec2 _vSize)
{
    // ���� ���� ���� �ʱ�ȭ
    Clear();

    // List �̸�, ������, Ȱ��ȭ ���� ����
    SetName(_strName);
    SetSize(_vSize.x, _vSize.y);
    SetActive(true);

    // ��ġ ����
    HWND hWnd = ImGuiManager::GetInst()->GetMainHwnd();

    RECT rt = {};
    //GetClientRect(hWnd, &rt);         // Client : �����츸�� �������� �»���� 0, 0���� ���� �ػ󵵸�ŭ 
    GetWindowRect(hWnd, &rt);     // Window: ȭ�� ��ü�� �������� �������� LT�� �¿� �е��� �� ���� â ũ��

    // ȭ�� �߾� ��ǥ�� ���Ѵ�.
    float CenterX = (float)((rt.left + rt.right) / 2);
    float CenterY = (float)((rt.bottom + rt.top) / 2);

    // ã�� �߾���ǥ�� Popupâ�� �߾��� �� �� �ְ� ����,���� ������ ������ ���ش�.
    SetPopupPos(ImVec2(CenterX - _vSize.x / 2.f, CenterY - _vSize.y / 2.f));
}
