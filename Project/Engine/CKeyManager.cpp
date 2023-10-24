#include "pch.h"
#include "CKeyManager.h"

#include "CLevelManager.h"
#include "CLevel.h"
#include "CGameObject.h"
#include "component.h"
#include "CEngine.h"

int g_arrVK[(UINT)KEY::END]
=
{
	 VK_UP, 
	 VK_DOWN,
	 VK_LEFT,
	 VK_RIGHT,

	 VK_SPACE,	 
	 VK_RETURN,
	 VK_ESCAPE,
	 VK_LMENU,
	 VK_LCONTROL,
	 VK_LSHIFT,
	 VK_TAB,	 
	 
	 VK_LBUTTON,
	 VK_RBUTTON,

	 'Q',
	 'W',
	 'E',
	 'R',

	 'A',
	 'S',
	 'D',
	 'F',

	 'Z',
	 'X',
	 'C',
	 'V',

	 'O',
	 'P',

	 '0',
	 '1',
	 '2',
	 '3',
	 '4',
	 '5',
	 '6',
	 '7',
	 '8',
	 '9',

};





CKeyManager::CKeyManager()
{

}

CKeyManager::~CKeyManager()
{

}


void CKeyManager::init()
{
	for (int i = 0; i < (int)KEY::END; ++i)
	{
		m_vecKey.push_back(tKeyInfo{ (KEY)i  , KEY_STATE::NONE });
	}
}

void CKeyManager::tick()
{
	if (GetFocus())
	{
		for (size_t i = 0; i < m_vecKey.size(); ++i)
		{
			if (GetAsyncKeyState(g_arrVK[(UINT)m_vecKey[i].key]) & 0x8000)
			{
				// �������� ������ �ʾҴ�.
				if (false == m_vecKey[i].bPrev)
				{
					m_vecKey[i].state = KEY_STATE::TAP;
					m_vecKey[i].bPrev = true;
				}
				else
				{
					// ���ݵ� �����ְ�, ���� �����ӿ����� �����־���.
					m_vecKey[i].state = KEY_STATE::PRESSED;
				}
			}
			else
			{
				// �������� �ʴ�.
				if (false == m_vecKey[i].bPrev)
				{
					m_vecKey[i].state = KEY_STATE::NONE;
				}
				else
				{
					m_vecKey[i].state = KEY_STATE::RELEASE;
					m_vecKey[i].bPrev = false;
				}
			}
		}

		// Mouse ��ġ ����
		m_vPrevMousePos = m_vMousePos;		// ���� �� ���� ������ ���콺 ��ġ ����

		POINT ptMousePos = {};
		GetCursorPos(&ptMousePos);		
		ScreenToClient(CEngine::GetInst()->GetMainWnd(), &ptMousePos);
		m_vMousePos = Vec2((float)ptMousePos.x, (float)ptMousePos.y);

		m_vMouseDir = m_vMousePos - m_vPrevMousePos;		// ���� ���콺 ��ǥ���� ���� ������ ���콺 ��ǥ�� �� ���� ���콺�� �̵�����
		m_vMouseDir.y *= -1;			// Window�� ���̷�Ʈ�� y���� ���� �ݴ��̹Ƿ� -1�� ���ؼ� �������ش�
	}

	// Window �� focus ���°� �ƴϴ�
	else
	{
		for (size_t i = 0; i < m_vecKey.size(); ++i)
		{
			if (KEY_STATE::TAP == m_vecKey[i].state || KEY_STATE::PRESSED == m_vecKey[i].state)
			{
				m_vecKey[i].state = KEY_STATE::RELEASE;
			}

			else if (KEY_STATE::RELEASE == m_vecKey[i].state)
			{
				m_vecKey[i].state = KEY_STATE::NONE;
			}			 
		}
	}	
}

Vec2 CKeyManager::GetProjMousePos()
{
	Vec2 MousePos = GetMousePos();

	// Ŭ���̾�Ʈ ���� ��ǥ�� -1 ~ 1 ������ ����ȭ
	Vec2 vResolution = CEngine::GetInst()->GetWindowResolution();
	Vec2 Normalized = Vec2((MousePos.x / vResolution.x) * 2.f - 1.f, (MousePos.y / vResolution.y) * -2.f + 1.f);

	CGameObject* Camera = CLevelManager::GetInst()->GetCurLevel()->FindObjectByName(L"MainCamera");
	Matrix ViewMat = Camera->Camera()->GetViewMat();
	
	// ��������� ������ ���� ���� �� ���� ��ǥ�� �����Ѵ�.
	float m_fScale = 1.f;
	Matrix ProjMat = XMMatrixOrthographicLH(vResolution.x * (1.f / m_fScale)
		, vResolution.y * (1.f / m_fScale)
		, 1.f
		, 10000.f);

	// ��*������ǥ�� ���ϱ� ���� ���� ���Ѵ�.
	Matrix ViewProjMat = XMMatrixMultiply(ViewMat, ProjMat);

	// ������ ��*������ǥ�� ������� ���Ѵ�.
	Matrix InverseViewProjMat = XMMatrixInverse(nullptr, ViewProjMat);

	Vec4 NormalizedMousePos = XMVectorSet(Normalized.x, Normalized.y, 0.0f, 1.0f);
	Vec4 WorldPos = XMVector4Transform(NormalizedMousePos, InverseViewProjMat);

	MousePos = Vec2(XMVectorGetX(WorldPos), XMVectorGetY(WorldPos));
	return MousePos;
}
