#include "pch.h"
#include "MeshUI.h"

#include <Engine\CMesh.h>

MeshUI::MeshUI()
    : ResourceUI(RES_TYPE::MESH)
{
    SetName("Mesh");
}

MeshUI::~MeshUI()
{
}

int MeshUI::render_update()
{
    ResourceUI::render_update();

    Ptr<CMesh> pMesh = (CMesh*)GetTargetRes().Get();

    string strKey = string(pMesh->GetKey().begin(), pMesh->GetKey().end());

    ImGui::InputText("##MeshUIName", (char*)strKey.c_str(), ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);

    return 0;
}
