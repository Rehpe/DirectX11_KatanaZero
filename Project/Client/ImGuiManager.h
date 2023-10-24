#pragma once
#include <Engine\CSingleton.h>

#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_dx11.h"
#include "ImGui\imgui_impl_win32.h"

// ===========DELEGATE 용 타입(전역변수)===============
class UI;

typedef void (UI::* UI_DELEGATE)(void);                     // 반환타입 void, 인자 없음
typedef void (UI::* UI_DELEGATE_1)(DWORD_PTR);              // 반환타입 void, 인자 하나
typedef void (UI::* UI_DELEGATE_2)(DWORD_PTR, DWORD_PTR);   // 반환타입 void, 인자 둘

class ImGuiManager :
    public CSingleton<ImGuiManager>
{
    SINGLETON(ImGuiManager);

private:
    HWND                m_hMainHwnd;
    map<string, UI*>    m_mapUI;        // UI를 단일 포인터가 아닌, UI타입으로 Map으로 관리

    HANDLE              m_hObserver;    // Content 폴더 내의 파일들의 변경을 감지하는 감시자

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

