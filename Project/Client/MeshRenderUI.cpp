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
	// �θ��� render_update ���н� ���� ��ȯ
	if (FALSE == ComponentUI::render_update())
		return FALSE;

	char szBuff[50] = {};

	Ptr<CMesh> pMesh = GetTarget()->MeshRender()->GetMesh();
	Ptr<CMaterial> pMtrl = GetTarget()->MeshRender()->GetMaterial();

	ImGui::Text("Mesh    ");
	ImGui::SameLine();
	GetResKey(pMesh.Get(), szBuff, 50);
	// �޽��� ������ �̸��� ������ �� ���� ReadOnly Flag ����
	ImGui::InputText("##MeshName", szBuff, 50, ImGuiInputTextFlags_ReadOnly);

	// Mesh ��� üũ
	if (ImGui::BeginDragDropTarget())
	{
		// �ش� ��忡�� ���콺 �� ���, ������ PayLoad Ű���� ��ġ�� ���
		const ImGuiPayload* pPayLoad = ImGui::AcceptDragDropPayload("Resource");
		if (pPayLoad)
		{
			TreeNode* pNode = (TreeNode*)pPayLoad->Data;
			CResource* pResource = (CResource*)pNode->GetData();
			// ���ҽ� Ÿ���� Mesh�� ��쿡�� SetMesh�Լ� ȣ��
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
			//MapMesh ���� Ű������ pListUI�� ��Ʈ�� ���� ���� ����ִ´�.
			pListUI->AddItem(string(pair.first.begin(), pair.first.end()));
		}

		// �׸� ���ý� ȣ����� ��������Ʈ ���
		pListUI->AddDynamic_Select(this, (UI_DELEGATE_1)&MeshRenderUI::SelectMesh);
	}

	ImGui::Text("Material");
	ImGui::SameLine();
	GetResKey(pMtrl.Get(), szBuff, 50);
	// �޽��� ������ �̸��� ������ �� ���� ReadOnly Flag ����
	ImGui::InputText("##pMtrlName", szBuff, 50, ImGuiInputTextFlags_ReadOnly);

	// Mateiral ��� üũ
	if (ImGui::BeginDragDropTarget())
	{
		// �ش� ��忡�� ���콺 �� ���, ������ PayLoad Ű���� ��ġ�� ���
		const ImGuiPayload* pPayLoad = ImGui::AcceptDragDropPayload("Resource");
		if (pPayLoad)
		{
			TreeNode* pNode = (TreeNode*)pPayLoad->Data;
			CResource* pResource = (CResource*)pNode->GetData();
			// ���ҽ� Ÿ���� Mateiral�� ��쿡�� SetMesh�Լ� ȣ��
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
			//MapMtrl ���� Ű������ pListUI�� ��Ʈ�� ���� ���� ����ִ´�.
			pListUI->AddItem(string(pair.first.begin(), pair.first.end()));
		}

		// �׸� ���ý� ȣ����� ��������Ʈ ���
		pListUI->AddDynamic_Select(this, (UI_DELEGATE_1)&MeshRenderUI::SelectMaterial);
	}

	return TRUE;
}

void MeshRenderUI::SelectMesh(DWORD_PTR _Key)
{
	// ���ڷ� ���� Ű������ ���ҽ��Ŵ����� �ʿ��� �ش� Mesh�� ã�� Ÿ���� �޽� ����
	string strKey = (char*)_Key;
	Ptr<CMesh> pMesh = CResourceManager::GetInst()->FindRes<CMesh>(wstring(strKey.begin(), strKey.end()));
	GetTarget()->MeshRender()->SetMesh(pMesh);
}

void MeshRenderUI::SelectMaterial(DWORD_PTR _Key)
{
	// ���ڷ� ���� Ű������ ���ҽ��Ŵ����� �ʿ��� �ش� Material�� ã�� Ÿ���� �޽� ����
	string strKey = (char*)_Key;
	Ptr<CMaterial> pMaterial = CResourceManager::GetInst()->FindRes<CMaterial>(wstring(strKey.begin(), strKey.end()));
	GetTarget()->MeshRender()->SetMaterial(pMaterial);
}
