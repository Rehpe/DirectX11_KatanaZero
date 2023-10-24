#include "pch.h"
#include "Light2DUI.h"

#include <Engine\CGameObject.h>
#include <Engine\CLight2D.h>
#include <Engine\CTransform.h>

Light2DUI::Light2DUI()
    : ComponentUI("##Light2D", COMPONENT_TYPE::LIGHT2D)
{
    SetName("Light2D");
}

Light2DUI::~Light2DUI()
{
}

int Light2DUI::render_update()
{
    if (FALSE == ComponentUI::render_update())
        return FALSE;

    
    // 표시할 변수 가져오기
    int iLightType = GetTarget()->Light2D()->GetLightType();
    Vec3 vLightDiffuse = GetTarget()->Light2D()->GetLightDiffuse();
    Vec3 vLightAmbient = GetTarget()->Light2D()->GetLightAmbient();
    float fIntensity = GetTarget()->Light2D()->GetIntensity();
    float fRadius = GetTarget()->Light2D()->GetRadius();
    float fAngle = GetTarget()->Light2D()->GetAngle();
    fAngle = (fAngle / XM_PI) * 180.f;

    ImGui::Text("LightType");
    ImGui::SameLine();
    const char* LightType[] = { "Directional Light", "Point Light", "Spot Light"};
    if (ImGui::Combo("##LightType", &iLightType, LightType, IM_ARRAYSIZE(LightType))) 
    {
        GetTarget()->Light2D()->SetLightType((LIGHT_TYPE)iLightType);
    }

    static bool alpha_preview = true;
    static bool alpha_half_preview = false;
    static bool drag_and_drop = true;
    static bool options_menu = true;
    static bool hdr = false;
   
    ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

    static Vec3 originalLightDiffuse = vLightDiffuse;
    static Vec3 originalLightAmbient = vLightAmbient;

    ImGui::Text("Diffuse  ");
    ImGui::SameLine();
    if (ImGui::ColorEdit3("##Diffuse", (float*)&vLightDiffuse, ImGuiColorEditFlags_Float | misc_flags))
    {
        GetTarget()->Light2D()->SetLightDiffuse(vLightDiffuse);
        originalLightDiffuse = vLightDiffuse;
    }
    
    ImGui::Text("Ambient  ");
    ImGui::SameLine();
    if (ImGui::ColorEdit3("##Ambient", (float*)&vLightAmbient, ImGuiColorEditFlags_Float | misc_flags))
    {
        GetTarget()->Light2D()->SetLightAmbient(vLightAmbient);
        originalLightAmbient = vLightAmbient;
    }

    ImGui::Text("Intensity");
    ImGui::SameLine();
    if (ImGui::SliderFloat("##Intensity", &fIntensity, 0.f, 10.f, "%.2f"))
    {
        GetTarget()->Light2D()->SetIntensity(fIntensity);
        GetTarget()->Light2D()->SetLightDiffuse(originalLightDiffuse * fIntensity);
        GetTarget()->Light2D()->SetLightAmbient(originalLightAmbient * fIntensity);
    }

    ImGui::Text("Radius   ");
    ImGui::SameLine();
    ImGui::DragFloat("##LightRadis", &fRadius, 1.f, 0.f);

    ImGui::Text("Angle    ");
    ImGui::SameLine();
    ImGui::DragFloat("##LightAngle", &fAngle, 1.f, 0.f, 180.f);


    Vec3 StartPos = GetTarget()->Transform()->GetRelativePos();

    if(iLightType == 1)
        DrawDebugCircle(StartPos, fRadius, Vec4(0.f, 0.f, 1.f, 1.f), Vec3(0.f, 0.f, 0.f));

    if (iLightType == 2)
    {

    }

    // 게임에 적용
    
    
    
    GetTarget()->Light2D()->SetRadius(fRadius);
    fAngle = (fAngle / 180) * XM_PI;
    GetTarget()->Light2D()->SetAngle(fAngle);



    return TRUE;
}
