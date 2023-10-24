#include "pch.h"
#include "ComponentUI.h"

#include <Engine\CGameObject.h>

ComponentUI::ComponentUI(const string& _Name, COMPONENT_TYPE _Type)
	: UI(_Name)
	, m_Type(_Type)
{
}

ComponentUI::~ComponentUI()
{
}

void ComponentUI::init()
{
}

int ComponentUI::render_update()
{
	// 타겟 오브젝트가 없거나, 타겟 오브젝트에 해당 컴포넌트가 없다면 실패
	if (nullptr == m_Target || nullptr == m_Target->GetComponent(m_Type))
		return FALSE;

	ImGui::PushID(0);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f / 7.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.f / 7.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.f / 7.0f, 0.6f, 0.6f));
	ImGui::Button(GetName().c_str());
	ImGui::PopStyleColor(3);
	ImGui::PopID();

	return TRUE;
}

void ComponentUI::SetTarget(CGameObject* _Target)
{
	m_Target = _Target;

	// 타겟이 없으면
	if (nullptr == m_Target)
	{
		// 모든 ComponentUI를 끄고 return한다 (아무 창도 뜨지 않음)
		SetActive(false);
		return;
	}
	// 타겟 오브젝트에 해당 타입 컴포넌트가 없다면 Active 비활성화
	if (nullptr == m_Target->GetComponent(m_Type))
	{
		SetActive(false);
	}
	else
	{
		SetActive(true);
		init();
	}
}


void ComponentUI::GetResKey(Ptr<CResource> _Resource, char* _Buff, size_t _BufferSize)
{
	// 버퍼를 0으로 초기화
	memset(_Buff, 0, sizeof(char) * _BufferSize);

	// 해당 리소스가 없다면 리턴
	if (nullptr == _Resource)
		return;

	wstring wstrKey = _Resource->GetKey();
	string	strKey = string(wstrKey.begin(), wstrKey.end());

	// 버퍼에 1바이트 문자열로 변환한 문자열을 memcpy
	memcpy(_Buff, strKey.data(), sizeof(char) * strKey.length());
}
