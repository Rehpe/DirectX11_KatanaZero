#include "pch.h"
#include "UI.h"

UI::UI(const string& _ID)
	: m_strID(_ID)
	, m_ParentUI(nullptr)
	, m_Modal(false)		// �⺻�� ��޸���
	, m_Active(false)		// �⺻�� ��Ȱ��ȭ
{
}

UI::~UI()
{
	Safe_Del_Vec(m_vecChildUI);
}

void UI::finaltick()
{
	// Ȱ��ȭ���� ���� ��쿣 return
	if (!m_Active)
		return;

	// �������� �̸� + ���� ID���� ������ �̸��� �ȴ�.
	string strFullName = m_strName + m_strID;

	// �θ� ����, �� ������ �ֻ��� �θ� UI�̴�
	if (nullptr == m_ParentUI)
	{
		// ��޸���
		if (!m_Modal)
		{
			ImGui::Begin(strFullName.c_str(), &m_Active);

			render_update();

			for (size_t i = 0; i < m_vecChildUI.size(); ++i)
			{
				// �ڽ�UI �� ��Ȱ��ȭ ���¸� �ǳʶڴ�.
				if (!m_vecChildUI[i]->IsActive())
					continue;

				// Ȱ��ȭ ������ ���� �ڽ� UI finaltick, ���м� ���
				m_vecChildUI[i]->finaltick();

				if (i != m_vecChildUI.size() - 1)
					ImGui::Separator();
			}

			ImGui::End();
		}
		// ���
		else
		{
			//ImGui::SetNextWindowPos(m_vPopupPos);		// �˾�â�� ȭ�� �߾ӿ� ���
			ImGui::SetNextWindowSize(m_vSize);

			// ��� ����� ��� OpenPopup(), BeginPopupModal(), endPopUp() �Լ� ���
			ImGui::OpenPopup(strFullName.c_str());
			if (ImGui::BeginPopupModal(strFullName.c_str(), &m_Active))
			{
				render_update();

				for (size_t i = 0; i < m_vecChildUI.size(); ++i)
				{
					// �ڽ�UI �� ��Ȱ��ȭ ���¸� �ǳʶڴ�.
					if (!m_vecChildUI[i]->IsActive())
						continue;

					// Ȱ��ȭ ������ ���� �ڽ� UI finaltick, ���м� ���
					m_vecChildUI[i]->finaltick();

					if (i != m_vecChildUI.size() - 1)
						ImGui::Separator();
				}

				ImGui::EndPopup();
			}
		}
	}
	// �θ� �ִ�, �� �ڽ� UI�̴�
	else
	{
		// �ڽ��� ��� ��޹���� �ƴ� BeginChild ��� ����ϹǷ� ��� �б��� ����
		
		// BeginChild : �ڽ� UI ���� �Լ�
		ImGui::BeginChild(strFullName.c_str(), m_vSize);

		render_update();

		// �ڽĿ��Ե� �ڽ��� ���� �� �����Ƿ�, �ڽ� ����ŭ �ڽĿ��� finaltick ȣ��
		for (size_t i = 0; i < m_vecChildUI.size(); ++i)
		{
			m_vecChildUI[i]->finaltick();

			if (i != m_vecChildUI.size() - 1)
				ImGui::Separator();
		}

		ImGui::EndChild();
	}
}

void UI::WstringtoString(wstring _Key, char* _Buff, size_t _BufferSize)
{
	// ���۸� 0���� �ʱ�ȭ
	memset(_Buff, 0, sizeof(char) * _BufferSize);

	string	strKey = string(_Key.begin(), _Key.end());

	// ���ۿ� 1����Ʈ ���ڿ��� ��ȯ�� ���ڿ��� memcpy
	memcpy(_Buff, strKey.data(), sizeof(char) * strKey.length());
}
