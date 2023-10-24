#include "pch.h"
#include "ParamUI.h"

#include "ImGuiManager.h"
#include "ListUI.h"
#include "TreeUI.h"

#include <Engine\CResourceManager.h>

// 정적맴버 초기화
UINT ParamUI::g_NextID = 0;

int ParamUI::Param_Int(const string& _strDesc, int* _pData, bool _bDrag)
{
    // Desc(파라미터 설명) 출력
    ImGui::Text(_strDesc.c_str());
    ImGui::SameLine(100);

    // 뒤에 숫자 붙인 라벨값 제작
    string strIntName = GetNextName("##Param_Int");
    ImGui::SetNextItemWidth(150);

    if (_bDrag)
    {
        if (ImGui::DragInt(strIntName.c_str(), _pData))
        {
            return 1;
        }
    }

    else
    {
        if (ImGui::InputInt(strIntName.c_str(), _pData))
        {
            return 1;
        }
    }

    return 0;
}

int ParamUI::Param_Float(const string& _strDesc, float* _pData, bool _bDrag)
{
    ImGui::Text(_strDesc.c_str());
    ImGui::SameLine(100);

    string strIntName = GetNextName("##Param_Float");
    ImGui::SetNextItemWidth(150);

    if (_bDrag)
    {
        if (ImGui::DragFloat(strIntName.c_str(), _pData))
        {
            return 1;
        }
    }

    else
    {
        if (ImGui::InputFloat(strIntName.c_str(), _pData))
        {
            return 1;
        }
    }

	return 0;
}

int ParamUI::Param_Vec2(const string& _strDesc, Vec2* _pData, bool _bDrag)
{
    ImGui::Text(_strDesc.c_str());
    ImGui::SameLine(100);

    string strIntName = GetNextName("##Param_Vec2");
    ImGui::SetNextItemWidth(150);

    float arrFloat[2] = { _pData->x, _pData->y };

    if (_bDrag)
    {
        if (ImGui::DragFloat2(strIntName.c_str(), arrFloat))
        {
            _pData->x = arrFloat[0];
            _pData->y = arrFloat[1];
            return 1;
        }
    }

    else
    {
        if (ImGui::InputFloat2(strIntName.c_str(), arrFloat))
        {
            _pData->x = arrFloat[0];
            _pData->y = arrFloat[1];
            return 1;
        }
    }

    return 0;
}

int ParamUI::Param_Vec4(const string& _strDesc, Vec4* _pData, bool _bDrag)
{
    ImGui::Text(_strDesc.c_str());
    ImGui::SameLine(100);

    string strIntName = GetNextName("##Param_Vec4");
    ImGui::SetNextItemWidth(200);


    if (_bDrag)
    {
        if (ImGui::DragFloat4(strIntName.c_str(), *_pData))
        {
            return 1;
        }
    }

    else
    {
        if (ImGui::InputFloat4(strIntName.c_str(), *_pData))
        {
            return 1;
        }
    }

    return 0;
}

int ParamUI::Param_Tex(const string& _strDesc, Ptr<CTexture>& _CurTex, UI* _UI, UI_DELEGATE_1 _Func)
{
    ImGui::Text(_strDesc.c_str());

    string strIntName = GetNextName("##Param_Tex");

    ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
    ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
    ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
    ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white

    if (_CurTex == nullptr)
    {
        // 텍스처 바인딩이 안 되어있을 경우 검정색 사각형을 출력한다
        ImGui::Image((ImTextureID)0, ImVec2(150, 150), uv_min, uv_max, tint_col, border_col);
    }
    else
    {
        ImGui::Image((ImTextureID)_CurTex->GetSRV().Get(), ImVec2(150, 150), uv_min, uv_max, tint_col, border_col);
    }

    // Texture 드랍 체크
    if (ImGui::BeginDragDropTarget())
    {
        // 해당 노드에서 마우스 뗀 경우, 지정한 PayLoad 키값이 일치한 경우
        const ImGuiPayload* pPayLoad = ImGui::AcceptDragDropPayload("Resource");
        if (pPayLoad)
        {
            TreeNode* pNode = (TreeNode*)pPayLoad->Data;
            CResource* pResource = (CResource*)pNode->GetData();
            if (RES_TYPE::TEXTURE == pResource->GetType())
            {
                _CurTex = (CTexture*)pResource;
            }
        }

        ImGui::EndDragDropTarget();

        return 0;
    }

    ImGui::SameLine();

    string strBtnName = GetNextName("##TexSelectBtn");
    if (ImGui::Button(strBtnName.c_str(), ImVec2(18, 18)))
    {
        // 버튼을 누르면 현재 로딩된 텍스처 리소스를 받아온다
        const map<wstring, Ptr<CResource>>& mapTex = CResourceManager::GetInst()->GetResources(RES_TYPE::TEXTURE);

        // ListUI 받아와 리스트에 텍스처들을 추가한다.
        ListUI* pListUI = (ListUI*)ImGuiManager::GetInst()->FindUI("##List");
        pListUI->Reset("Texture", ImVec2(300.f, 500.f));
        for (const auto& pair : mapTex)
        {
            pListUI->AddItem(string(pair.first.begin(), pair.first.end()));
        }

        // 항목 선택시 호출받을 델리게이트 등록
        pListUI->AddDynamic_Select(_UI, _Func);

        return 1;
    }

    return 0;
}

string ParamUI::GetNextName(const string& _ShowName)
{
    char szKey[255] = {};
    strcpy_s(szKey, 255, _ShowName.c_str());

    char szID[50] = "##";
    char* dest = szID;
    _itoa_s(g_NextID++, dest + 2, 50 - 2, 10);

    strcat_s(szKey, 50, szID);

    return szKey;
}
