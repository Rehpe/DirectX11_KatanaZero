#include "pch.h"
#include "CEngine.h"

#include "CDevice.h"
#include "Test.h"

#include "CPathManager.h"
#include "CTimeManager.h"
#include "CKeyManager.h"
#include "CResourceManager.h"
#include "CLevelManager.h"
#include "CRenderManager.h"
#include "CEventManager.h"
#include "CCollisionManager.h"
#include "CFontManager.h"
#include "CRecordManager.h"
#include "CSound.h"

static const int forceFrameRate = 60.f;

CEngine::CEngine()
	:m_hWnd(nullptr)
{
}

CEngine::~CEngine()
{
}

int CEngine::init(HWND _hWnd, UINT _iWidth, UINT _iHeight)
{

	//메인 윈도우 핸들 받아옴
	m_hWnd = _hWnd;
	m_vResolution = Vec2((float)_iWidth, (float)_iHeight);

	ShowCursor(FALSE);

	//해상도에 맞는 작업영역 크기 조정
	RECT rt = { 0,0,(int)_iWidth,(int)_iHeight};
	AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, false);
	SetWindowPos(m_hWnd, nullptr, 10, 10, rt.right - rt.left, rt.bottom - rt.top, 0);
	ShowWindow(m_hWnd, true);

	// Device 초기화
	if (FAILED(CDevice::GetInst()->init(m_hWnd, _iWidth, _iHeight)))
	{
		MessageBox(nullptr, L"Device 초기화 실패", L"에러", MB_OK);
		return E_FAIL;
	}

	// Manager 초기화
	CPathManager::GetInst()->init();
	CKeyManager::GetInst()->init();
	CTimeManager::GetInst()->init();
	CResourceManager::GetInst()->init();
	CRenderManager::GetInst()->init();
	CFontManager::GetInst()->init();
	CLevelManager::GetInst()->init();

	return S_OK;
}

void CEngine::progress()
{
	tick();
	render();

	// 객체의 생성, 소멸 등의 중요한 일은 tick-render가 전부 끝난 후에 이벤트 매니저에서 처리된다.
	CEventManager::GetInst()->tick();
}

void CEngine::tick()
{
	// Manager Tick
	CResourceManager::GetInst()->tick();
	CTimeManager::GetInst()->tick();
	CKeyManager::GetInst()->tick();

	// FMOD Update
	CSound::g_pFMOD->update();

	CLevelManager::GetInst()->tick();
	CRecordManager::GetInst()->tick();
	CCollisionManager::GetInst()->tick();
}

void CEngine::render()
{
	// 렌더매니저 생성 이후로는 더 이상 레벨매니저는 물체의 렌더링을 담당하지 않고,
	// 렌더 매니저가 카메라가 분류한 물체들을 Sort 후에 순서대로 렌더링한다
	// Clear도 렌더매니저에서 담당.
	CRenderManager::GetInst()->render();

	// 기존 Engine에서 Present가 호출되는 구조라면 EditorObjMgr이 렌더 요청한 디버그쉐이프 렌더가
	// 보여지기 전에 Clear되어버려 씹히는 현상이 발생하므로, Present 함수를 Main함수로 꺼냄
	CTimeManager::GetInst()->render();
	CFontManager::GetInst()->render();
}
