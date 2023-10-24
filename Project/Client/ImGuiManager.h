#pragma once
#include <Engine\CSingleton.h>

#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_dx11.h"
#include "ImGui\imgui_impl_win32.h"

// ===========DELEGATE �� Ÿ��(��������)===============
class UI;

typedef void (UI::* UI_DELEGATE)(void);                     // ��ȯŸ�� void, ���� ����
typedef void (UI::* UI_DELEGATE_1)(DWORD_PTR);              // ��ȯŸ�� void, ���� �ϳ�
typedef void (UI::* UI_DELEGATE_2)(DWORD_PTR, DWORD_PTR);   // ��ȯŸ�� void, ���� ��

class ImGuiManager :
    public CSingleton<ImGuiManager>
{
    SINGLETON(ImGuiManager);

private:
    HWND                m_hMainHwnd;
    map<string, UI*>    m_mapUI;        // UI�� ���� �����Ͱ� �ƴ�, UIŸ������ Map���� ����

    HANDLE              m_hObserver;    // Content ���� ���� ���ϵ��� ������ �����ϴ� ������

public:
    void init(HWND _hWnd);
    void progress();

public:
    UI* FindUI(const string& _UIName);
    HWND GetMainHwnd() { return m_hMainHwnd; }

private:
    void CreateUI();
    void ObserveContent();

    void begin();
    void tick();        
    void finaltick();
    void render();




};

