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

    // UI ����
    Safe_Del_Map(m_mapUI);
}

void ImGuiManager::init(HWND _hWnd)
{
    m_hMainHwnd = _hWnd;

    // ImGui �ʱ�ȭ
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

    // Content ���� ���� ������ �������� ������ ������ �ڵ� ����
    wstring strContentPath = CPathManager::GetInst()->GetContentPath();

    // 1�� ����: ������ ��� 
    // 2�� ����: ����Ʈ������ ��� ������ ������ ����
    // 3�� ����: ������ �̺�Ʈ ����(��Ʈ�������� �־���, ����/������ ���� �� �߰�,����)
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

    // �� ������ Content ���� ���� ����
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
    // ����� UI map�� second(UI �ּ�)�� tick ȣ��
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
            // Ȱ��ȭ ������ InspectorUI���Ը� finaltick ȣ��
            pair.second->finaltick();
        }
    }

    // ����Ű�� ������ UIâ ��Ŀ���� �����Ѵ�.
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
    pUI->SetActive(true);   // �⺻ ������ ��Ȱ��ȭ�̹Ƿ� �� true ����
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
    // �÷��׷� �־��� �̺�Ʈ�� �����ϴ� �Լ�
    // �ι�° ����: ���ð�(�� ������� �ʰ� �̺�Ʈ�� �߻��ϸ� �ϴ� ���, ������ ���´�� �ٷ� ��ȯ)
    DWORD dwWaitStatus = WaitForSingleObject(m_hObserver, 0);

    // ������ �����Ѵٸ�
    if (dwWaitStatus == WAIT_OBJECT_0)
    {
        // ContentUI���� Reload ȣ��
        ContentUI* UI = (ContentUI*)FindUI("##Content");
        UI->Reload();

        // ������ �ڵ��� �� �� ���� ��ȯ�� �ڿ��� �ٽ� ���� ���� �����Ƿ�, ��� �������� ������ �� �ְ� �Լ� ȣ��
        FindNextChangeNotification(m_hObserver);
    }
}

UI* ImGuiManager::FindUI(const string& _UIName)
{
    // Ű ������ UI ���� ��ȸ�Ͽ� �ش� UI�� ã�� ��ȯ�Ѵ�.
    map<string, UI*>::iterator iter = m_mapUI.find(_UIName);

    if (iter == m_mapUI.end())
        return nullptr;

    return iter->second;
}