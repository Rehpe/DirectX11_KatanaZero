#include "pch.h"
#include "ImGuiManager.h"

#include <Engine\CDevice.h>
#include <Engine\CKeyManager.h>
#include <Engine\CLevelManager.h>
#include <Engine\CPathManager.h>

#include <Engine\CGameObject.h>

#include "UI.h"
#include "ParamUI.h"



ImGuiManager::ImGuiManager()
    : m_hMainHwnd(nullptr)
    , m_hObserver(nullptr)
{

}

ImGuiManager::~ImGuiManager()
{
    // ImGui Release
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    // UI 삭제
    Safe_Del_Map(m_mapUI);
}

void ImGuiManager::init(HWND _hWnd)
{
    m_hMainHwnd = _hWnd;

    // ImGui 초기화
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;
    //io.ConfigViewportsNoDefaultParent = true;
    //io.ConfigDockingAlwaysTabBar = true;
    //io.ConfigDockingTransparentPayload = true;
    //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
    //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(m_hMainHwnd);
    ImGui_ImplDX11_Init(DEVICE, CONTEXT);

    CreateUI();

    // Content 폴더 내의 파일의 변경점을 감시할 감시자 핸들 생성
    wstring strContentPath = CPathManager::GetInst()->GetContentPath();

    // 1번 인자: 감시할 경로 
    // 2번 인자: 서브트리까지 모두 감시할 것인지 여부
    // 3번 인자: 감시할 이벤트 종류(비트연산으로 넣어줌, 파일/폴더명 변경 및 추가,삭제)
    m_hObserver = FindFirstChangeNotification(strContentPath.c_str(), true
        , FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME
        | FILE_ACTION_REMOVED | FILE_ACTION_ADDED);
}

void ImGuiManager::progress()
{
    begin();
    tick();
    finaltick();
    render();

    // 매 프레임 Content 폴더 변경 감시
    ObserveContent();
}

void ImGuiManager::begin()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::tick()
{
    // 등록한 UI map의 second(UI 주소)로 tick 호출
    for (const auto& pair : m_mapUI)
    {
        pair.second->tick();
    }

    ParamUI::g_NextID = 0;
}

void ImGuiManager::finaltick()
{
    // Demo UI
    ImGui::ShowDemoWindow();

    // InspectorUI 
    for (const auto& pair : m_mapUI)
    {
        if (pair.second->IsActive())
        {
            // 활성화 상태의 InspectorUI에게만 finaltick 호출
            pair.second->finaltick();
        }
    }

    // 엔터키를 누르면 UI창 포커스를 해제한다.
    if (KEY_TAP(KEY::ENTER))
        ImGui::SetWindowFocus(nullptr);
}

void ImGuiManager::render()
{
    // ImGui Rendering
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

#include "InspectorUI.h"
#include "ContentUI.h"
#include "OutlinerUI.h"
#include "ListUI.h"
#include "MenuUI.h"

void ImGuiManager::CreateUI()
{
    UI* pUI = nullptr;

    // InspectorUI
    pUI = new InspectorUI;
    pUI->SetActive(true);   // 기본 설정이 비활성화이므로 꼭 true 설정
    m_mapUI.insert(make_pair(pUI->GetID(), pUI));

    // ContentUI
    pUI = new ContentUI;
    pUI->SetActive(true);
    m_mapUI.insert(make_pair(pUI->GetID(), pUI));

    // Outliner
    pUI = new OutlinerUI;
    pUI->SetActive(true);
    m_mapUI.insert(make_pair(pUI->GetID(), pUI));

    // List UI
    pUI = new ListUI;
    pUI->SetModal(true);
    pUI->SetActive(false);
    m_mapUI.insert(make_pair(pUI->GetID(), pUI));

    // Menu
    pUI = new MenuUI;
    pUI->SetActive(true);
    m_mapUI.insert(make_pair(pUI->GetID(), pUI));

    for (const auto& pair : m_mapUI)
    {
        pair.second->init();
    }

}

void ImGuiManager::ObserveContent()
{
    // 플래그로 넣어준 이벤트를 감시하는 함수
    // 두번째 인자: 대기시간(즉 대기하지 않고 이벤트가 발생하면 하는 대로, 없으면 없는대로 바로 반환)
    DWORD dwWaitStatus = WaitForSingleObject(m_hObserver, 0);

    // 변경을 감지한다면
    if (dwWaitStatus == WAIT_OBJECT_0)
    {
        // ContentUI에게 Reload 호출
        ContentUI* UI = (ContentUI*)FindUI("##Content");
        UI->Reload();

        // 감시자 핸들은 한 번 값을 반환한 뒤에는 다시 일을 하지 않으므로, 계속 변경점을 감시할 수 있게 함수 호출
        FindNextChangeNotification(m_hObserver);
    }
}

UI* ImGuiManager::FindUI(const string& _UIName)
{
    // 키 값으로 UI 맵을 순회하여 해당 UI를 찾아 반환한다.
    map<string, UI*>::iterator iter = m_mapUI.find(_UIName);

    if (iter == m_mapUI.end())
        return nullptr;

    return iter->second;
}