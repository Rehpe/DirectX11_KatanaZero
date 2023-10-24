#include "pch.h"
#include "PrefabUI.h"

#include <Engine\CResourceManager.h>
#include <Engine\CPrefab.h>
#include <Engine\CLevelManager.h>
#include <Engine\CLevel.h>

#include "ImGuiManager.h"
#include "OutlinerUI.h"

PrefabUI::PrefabUI()
    : ResourceUI(RES_TYPE::PREFAB)
{
    SetName("Prefab");
}

PrefabUI::~PrefabUI()
{
}

int PrefabUI::render_update()
{
    ResourceUI::render_update();
    
    if (ImGui::Button("Add to Layer"))
    {
        Ptr<CPrefab> pPrefab = (CPrefab*)GetTargetRes().Get();
        
        pPrefab->AddToLayer(0);

        OutlinerUI* Outliner = (OutlinerUI*)ImGuiManager::GetInst()->FindUI("##Outliner");
        Outliner->ResetOutliner();
    }

    return 0;
}
