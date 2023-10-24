#pragma once
#include <Engine\ptr.h>
#include <Engine\CResource.h>

#include "ImGuiManager.h"

// �ϳ��� UIâ�� UI��ü�� �� ���̴�.

class UI
{
public:
	UI(const string& _ID);	// �����ڿ��� ���� ID���� ���ڷ� �޴´�(�������� �ʴ� strID)
	virtual ~UI();

private:
	string			m_strName;		// (����Ǵ�) Ÿ��Ʋ �̸�, 1����Ʈ ���ڿ�
	string			m_strID;		// ID �̸�

	ImVec2			m_vSize;		// UI ũ��
	ImVec2			m_vPopupPos;	// Popup UI ��ġ

	UI*				m_ParentUI;		// �θ� UI
	vector<UI*>		m_vecChildUI;	// �ڽ� UI ���

	bool			m_Modal;		// ��� or ��޸���
	bool			m_Active;		// UI Ȱ��ȭ üũ

public:
	virtual void init() {}
	virtual void tick() {}				// �� ������ UI�� �ؾ��� �� ó��
	virtual void finaltick();

	// ��ȯ�� int : TRUE, FALSE�� �޾� ������ ���θ� �Ǵ��� �б����� ������.
	virtual int render_update() = 0;	// ó���� �� �������� GUI�ڵ� �ۼ��Ͽ� UI ���

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

	// �ڽ� UI�϶����� ���, SetSize�� x���� 0���� �дٸ� �θ��� ũ�⸦ ����ϰڴٴ� ��
	void SetSize(float _width, float _height) { m_vSize = ImVec2(_width, _height); }

	// ���ڿ� ��ȯ �Լ�
	void WstringtoString(wstring _key, char* _Buff, size_t _BufferSize);
};

