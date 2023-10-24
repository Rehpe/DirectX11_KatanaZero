#include "pch.h"
#include "CameraUI.h"

#include <Engine\CGameObject.h>
#include <Engine\CCamera.h>

CameraUI::CameraUI()
    : ComponentUI("##Camera", COMPONENT_TYPE::CAMERA)
{
    SetName("Camera");
}

CameraUI::~CameraUI()
{
}

int CameraUI::render_update()
{
    if (FALSE == ComponentUI::render_update())
        return FALSE;

	PROJ_TYPE ProjType = GetTarget()->Camera()->GetProjType();
	int iProjType = (int)ProjType;

	ImGui::Text("Projection Type");
	ImGui::SameLine();
	const char* ProjTypes[] = { "ORTHOGRAPHIC", "PERSPECTIVE" };
	if (ImGui::Combo("##ProjectionType", &iProjType, ProjTypes, IM_ARRAYSIZE(ProjTypes)))
	{
		GetTarget()->Camera()->SetProjType((PROJ_TYPE)iProjType);
	}

	int CamIdx = GetTarget()->Camera()->GetCameraIndex();
	ImGui::Text("Camera Index");
	ImGui::SameLine();
	if (ImGui::InputInt("##CamIdx", &CamIdx))
	{
		GetTarget()->Camera()->SetCameraIndex(CamIdx);
	}

    UINT LayerMask = GetTarget()->Camera()->GetLayerMask();
    ImGui::InputScalar("Layer Mask", ImGuiDataType_U32, &LayerMask, nullptr, nullptr, "%08X", ImGuiInputTextFlags_ReadOnly);

    // Create checkboxes to control each layer's visibility
    for (int i = 0; i < 32; ++i) {
        bool visible = (LayerMask & (1 << i)) != 0;
        ImGui::PushID(i);
        if (ImGui::Checkbox("Visible", &visible)) {
            GetTarget()->Camera()->SetLayerMask(i, visible);
        }
        ImGui::PopID();
        ImGui::SameLine();
        ImGui::Text("Layer %d", i);
        if ((i + 1) % 4 != 0) {
            ImGui::SameLine();
        }
    }

    // Create buttons to set all layers to visible or non-visible
    if (ImGui::Button("Set All Visible")) {
        GetTarget()->Camera()->SetLayerMaskAll(true);
    }
    ImGui::SameLine();
    if (ImGui::Button("Set All Non-Visible")) {
        GetTarget()->Camera()->SetLayerMaskAll(false);
    }

    return TRUE;
}
