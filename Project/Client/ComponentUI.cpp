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
	// Ÿ�� ������Ʈ�� ���ų�, Ÿ�� ������Ʈ�� �ش� ������Ʈ�� ���ٸ� ����
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

	// Ÿ���� ������
	if (nullptr == m_Target)
	{
		// ��� ComponentUI�� ���� return�Ѵ� (�ƹ� â�� ���� ����)
		SetActive(false);
		return;
	}
	// Ÿ�� ������Ʈ�� �ش� Ÿ�� ������Ʈ�� ���ٸ� Active ��Ȱ��ȭ
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
	// ���۸� 0���� �ʱ�ȭ
	memset(_Buff, 0, sizeof(char) * _BufferSize);

	// �ش� ���ҽ��� ���ٸ� ����
	if (nullptr == _Resource)
		return;

	wstring wstrKey = _Resource->GetKey();
	string	strKey = string(wstrKey.begin(), wstrKey.end());

	// ���ۿ� 1����Ʈ ���ڿ��� ��ȯ�� ���ڿ��� memcpy
	memcpy(_Buff, strKey.data(), sizeof(char) * strKey.length());
}
