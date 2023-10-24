#pragma once
#include <Engine\ptr.h>
#include <Engine\CResource.h>

#include "ImGuiManager.h"

// 하나의 UI창이 UI객체가 될 것이다.

class UI
{
public:
	UI(const string& _ID);	// 생성자에서 고유 ID값을 인자로 받는다(보여지지 않는 strID)
	virtual ~UI();

private:
	string			m_strName;		// (노출되는) 타이틀 이름, 1바이트 문자열
	string			m_strID;		// ID 이름

	ImVec2			m_vSize;		// UI 크기
	ImVec2			m_vPopupPos;	// Popup UI 위치

	UI*				m_ParentUI;		// 부모 UI
	vector<UI*>		m_vecChildUI;	// 자식 UI 목록

	bool			m_Modal;		// 모달 or 모달리스
	bool			m_Active;		// UI 활성화 체크

public:
	virtual void init() {}
	virtual void tick() {}				// 매 프레임 UI가 해야할 일 처리
	virtual void finaltick();

	// 반환값 int : TRUE, FALSE로 받아 렌더링 여부를 판단할 분기점을 나눈다.
	virtual int render_update() = 0;	// 처리한 일 바탕으로 GUI코드 작성하여 UI 출력

public:
	void AddChildUI(UI* _UI)
	{
		_UI->m_ParentUI = this;
		m_vecChildUI.push_back(_UI);
	}

public:
	void SetActive(bool _Active) { m_Active = _Active; }
	bool IsActive() { return m_Active; }

	void SetModal(bool _Modal) { m_Modal = _Modal; }
	bool IsModal() { return m_Modal; }

	const string& GetName() { return m_strName; }
	void SetName(const string& _Name) { m_strName = _Name; }

	ImVec2 GetPopupPos() { return m_vPopupPos; }
	void SetPopupPos(ImVec2 _vPos) { m_vPopupPos = _vPos; }

	const string& GetID() { return m_strID; }

	// 자식 UI일때에만 사용, SetSize에 x값을 0으로 둔다면 부모의 크기를 사용하겠다는 것
	void SetSize(float _width, float _height) { m_vSize = ImVec2(_width, _height); }

	// 문자열 변환 함수
	void WstringtoString(wstring _key, char* _Buff, size_t _BufferSize);
};

