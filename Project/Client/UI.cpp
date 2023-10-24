#include "pch.h"
#include "UI.h"

UI::UI(const string& _ID)
	: m_strID(_ID)
	, m_ParentUI(nullptr)
	, m_Modal(false)		// 기본값 모달리스
	, m_Active(false)		// 기본값 비활성화
{
}

UI::~UI()
{
	Safe_Del_Vec(m_vecChildUI);
}

void UI::finaltick()
{
	// 활성화되지 않은 경우엔 return
	if (!m_Active)
		return;

	// 보여지는 이름 + 고유 ID값이 합쳐져 이름이 된다.
	string strFullName = m_strName + m_strID;

	// 부모가 없다, 즉 본인이 최상위 부모 UI이다
	if (nullptr == m_ParentUI)
	{
		// 모달리스
		if (!m_Modal)
		{
			ImGui::Begin(strFullName.c_str(), &m_Active);

			render_update();

			for (size_t i = 0; i < m_vecChildUI.size(); ++i)
			{
				// 자식UI 가 비활성화 상태면 건너뛴다.
				if (!m_vecChildUI[i]->IsActive())
					continue;

				// 활성화 상태일 때만 자식 UI finaltick, 구분선 출력
				m_vecChildUI[i]->finaltick();

				if (i != m_vecChildUI.size() - 1)
					ImGui::Separator();
			}

			ImGui::End();
		}
		// 모달
		else
		{
			//ImGui::SetNextWindowPos(m_vPopupPos);		// 팝업창은 화면 중앙에 뜬다
			ImGui::SetNextWindowSize(m_vSize);

			// 모달 방식의 경우 OpenPopup(), BeginPopupModal(), endPopUp() 함수 사용
			ImGui::OpenPopup(strFullName.c_str());
			if (ImGui::BeginPopupModal(strFullName.c_str(), &m_Active))
			{
				render_update();

				for (size_t i = 0; i < m_vecChildUI.size(); ++i)
				{
					// 자식UI 가 비활성화 상태면 건너뛴다.
					if (!m_vecChildUI[i]->IsActive())
						continue;

					// 활성화 상태일 때만 자식 UI finaltick, 구분선 출력
					m_vecChildUI[i]->finaltick();

					if (i != m_vecChildUI.size() - 1)
						ImGui::Separator();
				}

				ImGui::EndPopup();
			}
		}
	}
	// 부모가 있다, 즉 자식 UI이다
	else
	{
		// 자식의 경우 모달방식이 아닌 BeginChild 방식 사용하므로 모달 분기점 없음
		
		// BeginChild : 자식 UI 여는 함수
		ImGui::BeginChild(strFullName.c_str(), m_vSize);

		render_update();

		// 자식에게도 자식이 있을 수 있으므로, 자식 수만큼 자식에게 finaltick 호출
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
	// 버퍼를 0으로 초기화
	memset(_Buff, 0, sizeof(char) * _BufferSize);

	string	strKey = string(_Key.begin(), _Key.end());

	// 버퍼에 1바이트 문자열로 변환한 문자열을 memcpy
	memcpy(_Buff, strKey.data(), sizeof(char) * strKey.length());
}
