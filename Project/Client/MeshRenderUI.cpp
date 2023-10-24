#include "pch.h"
#include "MeshRenderUI.h"

#include <Engine\CMeshRender.h>
#include <Engine\CResourceManager.h>

#include "ImGuiManager.h"
#include "ListUI.h"
#include "TreeUI.h"


MeshRenderUI::MeshRenderUI()
	: ComponentUI("##MeshRender", COMPONENT_TYPE::MESHRENDER)
{
	SetName("MeshRender");
}

MeshRenderUI::~MeshRenderUI()
{
}

int MeshRenderUI::render_update()
{
	// 부모의 render_update 실패시 실패 반환
	if (FALSE == ComponentUI::render_update())
		return FALSE;

	char szBuff[50] = {};

	Ptr<CMesh> pMesh = GetTarget()->MeshRender()->GetMesh();
	Ptr<CMaterial> pMtrl = GetTarget()->MeshRender()->GetMaterial();

	ImGui::Text("Mesh    ");
	ImGui::SameLine();
	GetResKey(pMesh.Get(), szBuff, 50);
	// 메쉬와 재질의 이름을 변경할 수 없게 ReadOnly Flag 지정
	ImGui::InputText("##MeshName", szBuff, 50, ImGuiInputTextFlags_ReadOnly);

	// Mesh 드랍 체크
	if (ImGui::BeginDragDropTarget())
	{
		// 해당 노드에서 마우스 뗀 경우, 지정한 PayLoad 키값이 일치한 경우
		const ImGuiPayload* pPayLoad = ImGui::AcceptDragDropPayload("Resource");
		if (pPayLoad)
		{
			TreeNode* pNode = (TreeNode*)pPayLoad->Data;
			CResource* pResource = (CResource*)pNode->GetData();
			// 리소스 타입이 Mesh인 경우에만 SetMesh함수 호출
			if (RES_TYPE::MESH == pResource->GetType())
			{
				GetTarget()->MeshRender()->SetMesh((CMesh*)pResource);
			}
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();
	if (ImGui::Button("##MeshSelectBtn", ImVec2(18, 18)))
	{
		const map<wstring, Ptr<CResource>>& mapMesh = CResourceManager::GetInst()->GetResources(RES_TYPE::MESH);

		ListUI* pListUI = (ListUI*)ImGuiManager::GetInst()->FindUI("##List");

		pListUI->Reset("Mesh List", ImVec2(300.f, 500.f));
		for (const auto& pair : mapMesh)
		{
			//MapMesh 내의 키값들을 pListUI의 스트링 벡터 내에 집어넣는다.
			pListUI->AddItem(string(pair.first.begin(), pair.first.end()));
		}

		// 항목 선택시 호출받을 델리게이트 등록
		pListUI->AddDynamic_Select(this, (UI_DELEGATE_1)&MeshRenderUI::SelectMesh);
	}

	ImGui::Text("Material");
	ImGui::SameLine();
	GetResKey(pMtrl.Get(), szBuff, 50);
	// 메쉬와 재질의 이름을 변경할 수 없게 ReadOnly Flag 지정
	ImGui::InputText("##pMtrlName", szBuff, 50, ImGuiInputTextFlags_ReadOnly);

	// Mateiral 드랍 체크
	if (ImGui::BeginDragDropTarget())
	{
		// 해당 노드에서 마우스 뗀 경우, 지정한 PayLoad 키값이 일치한 경우
		const ImGuiPayload* pPayLoad = ImGui::AcceptDragDropPayload("Resource");
		if (pPayLoad)
		{
			TreeNode* pNode = (TreeNode*)pPayLoad->Data;
			CResource* pResource = (CResource*)pNode->GetData();
			// 리소스 타입이 Mateiral인 경우에만 SetMesh함수 호출
			if (RES_TYPE::MATERIAL == pResource->GetType())
			{
				GetTarget()->MeshRender()->SetMaterial((CMaterial*)pResource);
			}
		}

		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();
	if (ImGui::Button("##MtrlSelectBtn", ImVec2(18, 18)))
	{
		const map<wstring, Ptr<CResource>>& mapMtrl = CResourceManager::GetInst()->GetResources(RES_TYPE::MATERIAL);

		ListUI* pListUI = (ListUI*)ImGuiManager::GetInst()->FindUI("##List");

		pListUI->Reset("Material List", ImVec2(300.f, 500.f));
		for (const auto& pair : mapMtrl)
		{
			//MapMtrl 내의 키값들을 pListUI의 스트링 벡터 내에 집어넣는다.
			pListUI->AddItem(string(pair.first.begin(), pair.first.end()));
		}

		// 항목 선택시 호출받을 델리게이트 등록
		pListUI->AddDynamic_Select(this, (UI_DELEGATE_1)&MeshRenderUI::SelectMaterial);
	}

	return TRUE;
}

void MeshRenderUI::SelectMesh(DWORD_PTR _Key)
{
	// 인자로 들어온 키값으로 리소스매니저의 맵에서 해당 Mesh를 찾아 타겟의 메쉬 변경
	string strKey = (char*)_Key;
	Ptr<CMesh> pMesh = CResourceManager::GetInst()->FindRes<CMesh>(wstring(strKey.begin(), strKey.end()));
	GetTarget()->MeshRender()->SetMesh(pMesh);
}

void MeshRenderUI::SelectMaterial(DWORD_PTR _Key)
{
	// 인자로 들어온 키값으로 리소스매니저의 맵에서 해당 Material를 찾아 타겟의 메쉬 변경
	string strKey = (char*)_Key;
	Ptr<CMaterial> pMaterial = CResourceManager::GetInst()->FindRes<CMaterial>(wstring(strKey.begin(), strKey.end()));
	GetTarget()->MeshRender()->SetMaterial(pMaterial);
}
