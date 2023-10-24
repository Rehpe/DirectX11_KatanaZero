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

	//���� ������ �ڵ� �޾ƿ�
	m_hWnd = _hWnd;
	m_vResolution = Vec2((float)_iWidth, (float)_iHeight);

	ShowCursor(FALSE);

	//�ػ󵵿� �´� �۾����� ũ�� ����
	RECT rt = { 0,0,(int)_iWidth,(int)_iHeight};
	AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, false);
	SetWindowPos(m_hWnd, nullptr, 10, 10, rt.right - rt.left, rt.bottom - rt.top, 0);
	ShowWindow(m_hWnd, true);

	// Device �ʱ�ȭ
	if (FAILED(CDevice::GetInst()->init(m_hWnd, _iWidth, _iHeight)))
	{
		MessageBox(nullptr, L"Device �ʱ�ȭ ����", L"����", MB_OK);
		return E_FAIL;
	}

	// Manager �ʱ�ȭ
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

	// ��ü�� ����, �Ҹ� ���� �߿��� ���� tick-render�� ���� ���� �Ŀ� �̺�Ʈ �Ŵ������� ó���ȴ�.
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
	// �����Ŵ��� ���� ���ķδ� �� �̻� �����Ŵ����� ��ü�� �������� ������� �ʰ�,
	// ���� �Ŵ����� ī�޶� �з��� ��ü���� Sort �Ŀ� ������� �������Ѵ�
	// Clear�� �����Ŵ������� ���.
	CRenderManager::GetInst()->render();

	// ���� Engine���� Present�� ȣ��Ǵ� ������� EditorObjMgr�� ���� ��û�� ����׽����� ������
	// �������� ���� Clear�Ǿ���� ������ ������ �߻��ϹǷ�, Present �Լ��� Main�Լ��� ����
	CTimeManager::GetInst()->render();
	CFontManager::GetInst()->render();
}
