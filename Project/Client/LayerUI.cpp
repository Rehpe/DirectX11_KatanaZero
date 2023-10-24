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

	// 1. 현재 레벨의 레이어 정보를 불러온다
	CLevel* pCurLevel = CLevelManager::GetInst()->GetCurLevel();

	string CurLayerArrstr[MAX_LAYER] = {};
	//wstring CurLayerArrwstr[MAX_LAYER] = {};

	// 2. 레이어의 이름을 wstr->str->char* 순으로 변환하여 m_LayerNameVec에 저장한다.
	for (int i = 0; i < MAX_LAYER; i++)
	{
		char szBuff[50] = {};
		WstringtoString(pCurLevel->GetLayer(i)->GetName(), szBuff, 50);
		m_LayerNameVec[i] = szBuff;
	}

	// 3. m_LayerNameArr에 m_LayerNameVec의 c_str() 값을 복사한다.
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
	// 1. 현재 레벨의 레이어 정보를 불러온다
	CLevel* pCurLevel = CLevelManager::GetInst()->GetCurLevel();

	string CurLayerArrstr[MAX_LAYER] = {};
	//wstring CurLayerArrwstr[MAX_LAYER] = {};

	// 2. 레이어의 이름을 wstr->str->char* 순으로 변환하여 m_LayerNameVec에 저장한다.
	for (int i = 0; i < MAX_LAYER; i++)
	{
		char szBuff[50] = {};
		WstringtoString(pCurLevel->GetLayer(i)->GetName(), szBuff, 50);
		m_LayerNameVec[i] = szBuff;
	}

	// 3. m_LayerNameArr에 m_LayerNameVec의 c_str() 값을 복사한다.
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
