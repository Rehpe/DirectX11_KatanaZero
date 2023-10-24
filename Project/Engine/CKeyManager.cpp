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
				// 이전에는 눌리지 않았다.
				if (false == m_vecKey[i].bPrev)
				{
					m_vecKey[i].state = KEY_STATE::TAP;
					m_vecKey[i].bPrev = true;
				}
				else
				{
					// 지금도 눌려있고, 이전 프레임에서도 눌려있었다.
					m_vecKey[i].state = KEY_STATE::PRESSED;
				}
			}
			else
			{
				// 눌려있지 않다.
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

		// Mouse 위치 갱신
		m_vPrevMousePos = m_vMousePos;		// 갱신 전 이전 프레임 마우스 위치 저장

		POINT ptMousePos = {};
		GetCursorPos(&ptMousePos);		
		ScreenToClient(CEngine::GetInst()->GetMainWnd(), &ptMousePos);
		m_vMousePos = Vec2((float)ptMousePos.x, (float)ptMousePos.y);

		m_vMouseDir = m_vMousePos - m_vPrevMousePos;		// 현재 마우스 좌표에서 이전 프레임 마우스 좌표를 뺀 값은 마우스의 이동방향
		m_vMouseDir.y *= -1;			// Window와 다이렉트의 y축이 서로 반대이므로 -1을 곱해서 보정해준다
	}

	// Window 가 focus 상태가 아니다
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

	// 클라이언트 영역 좌표를 -1 ~ 1 범위로 정규화
	Vec2 vResolution = CEngine::GetInst()->GetWindowResolution();
	Vec2 Normalized = Vec2((MousePos.x / vResolution.x) * 2.f - 1.f, (MousePos.y / vResolution.y) * -2.f + 1.f);

	CGameObject* Camera = CLevelManager::GetInst()->GetCurLevel()->FindObjectByName(L"MainCamera");
	Matrix ViewMat = Camera->Camera()->GetViewMat();
	
	// 투영행렬을 가져와 역을 취한 뒤 최종 좌표를 산출한다.
	float m_fScale = 1.f;
	Matrix ProjMat = XMMatrixOrthographicLH(vResolution.x * (1.f / m_fScale)
		, vResolution.y * (1.f / m_fScale)
		, 1.f
		, 10000.f);

	// 뷰*투영좌표를 구하기 위해 둘을 곱한다.
	Matrix ViewProjMat = XMMatrixMultiply(ViewMat, ProjMat);

	// 구해진 뷰*투영좌표의 역행렬을 구한다.
	Matrix InverseViewProjMat = XMMatrixInverse(nullptr, ViewProjMat);

	Vec4 NormalizedMousePos = XMVectorSet(Normalized.x, Normalized.y, 0.0f, 1.0f);
	Vec4 WorldPos = XMVector4Transform(NormalizedMousePos, InverseViewProjMat);

	MousePos = Vec2(XMVectorGetX(WorldPos), XMVectorGetY(WorldPos));
	return MousePos;
}
