#include "pch.h"
#include "LayerUI.h"

#include <Engine\CLevelManager.h>
#include <Engine\CLevel.h>
#include <Engine\CGameObject.h>

LayerUI::LayerUI()
	: UI("##Layer")
	, m_pTargetObj(nullptr)
	, m_LayerNameArr{}
	, m_LayerNameVec(MAX_LAYER)
	, m_curLayerIdx(-1)

{
    SetName("Layer");

	// 1. ���� ������ ���̾� ������ �ҷ��´�
	CLevel* pCurLevel = CLevelManager::GetInst()->GetCurLevel();

	string CurLayerArrstr[MAX_LAYER] = {};
	//wstring CurLayerArrwstr[MAX_LAYER] = {};

	// 2. ���̾��� �̸��� wstr->str->char* ������ ��ȯ�Ͽ� m_LayerNameVec�� �����Ѵ�.
	for (int i = 0; i < MAX_LAYER; i++)
	{
		char szBuff[50] = {};
		WstringtoString(pCurLevel->GetLayer(i)->GetName(), szBuff, 50);
		m_LayerNameVec[i] = szBuff;
	}

	// 3. m_LayerNameArr�� m_LayerNameVec�� c_str() ���� �����Ѵ�.
	for (int i = 0; i < MAX_LAYER; i++)
	{
		m_LayerNameArr[i] = m_LayerNameVec[i].c_str();
	}
}

LayerUI::~LayerUI()
{
}

int LayerUI::render_update()
{
	// 1. ���� ������ ���̾� ������ �ҷ��´�
	CLevel* pCurLevel = CLevelManager::GetInst()->GetCurLevel();

	string CurLayerArrstr[MAX_LAYER] = {};
	//wstring CurLayerArrwstr[MAX_LAYER] = {};

	// 2. ���̾��� �̸��� wstr->str->char* ������ ��ȯ�Ͽ� m_LayerNameVec�� �����Ѵ�.
	for (int i = 0; i < MAX_LAYER; i++)
	{
		char szBuff[50] = {};
		WstringtoString(pCurLevel->GetLayer(i)->GetName(), szBuff, 50);
		m_LayerNameVec[i] = szBuff;
	}

	// 3. m_LayerNameArr�� m_LayerNameVec�� c_str() ���� �����Ѵ�.
	for (int i = 0; i < MAX_LAYER; i++)
	{
		m_LayerNameArr[i] = m_LayerNameVec[i].c_str();
	}

	// ===================
	if (m_pTargetObj == nullptr)
		return 0;

	m_curLayerIdx = m_pTargetObj->GetLayerIdx();

	ImGui::Text("Current Layer : ");

	ImGui::SetNextItemWidth(200);

	if (ImGui::Combo("##CurLayerList", &m_curLayerIdx, m_LayerNameArr, MAX_LAYER))
	{
		m_pTargetObj->ChangeLayer(m_curLayerIdx);
	}

    return 0;
}
