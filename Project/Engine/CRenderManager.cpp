#include "pch.h"
#include "CRenderManager.h"

#include "CCamera.h"
#include "CDevice.h"
#include "CConstBuffer.h"
#include "CStructuredBuffer.h"

#include "CResourceManager.h"

CRenderManager::CRenderManager()
	: m_Light2DBuffer(nullptr)
	, RENDER_FUNC(nullptr)
	, m_pEditorCam(nullptr)
{
	// ����Ÿ�� �ػ󵵿� ������ ����Ÿ�� ����� �ؽ�ó ����
	Vec2 vResolution = CDevice::GetInst()->GetRenderResolution();
	m_RTCopyTex = CResourceManager::GetInst()->CreateTexture(L"RTCopyTex"
				, (UINT)vResolution.x, (UINT)vResolution.y
				, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_SHADER_RESOURCE
				, D3D11_USAGE_DEFAULT);

	CResourceManager::GetInst()->FindRes<CMaterial>(L"GrayMtrl")->SetTexParam(TEX_0, m_RTCopyTex);
	CResourceManager::GetInst()->FindRes<CMaterial>(L"DistortionMtrl")->SetTexParam(TEX_0, m_RTCopyTex);
	CResourceManager::GetInst()->FindRes<CMaterial>(L"GlitchMtrl")->SetTexParam(TEX_0, m_RTCopyTex);
	CResourceManager::GetInst()->FindRes<CMaterial>(L"NewDistortionMtrl")->SetTexParam(TEX_0, m_RTCopyTex);
}

CRenderManager::~CRenderManager()
{
	// ����ȭ������ ��� �����ڰ� �����Ƿ� ��� �� ���� ���� �������־����
	if (nullptr != m_Light2DBuffer)
		delete m_Light2DBuffer;
}

void CRenderManager::init()
{
	// Light2DBuffer ����ȭ ���� ����
	m_Light2DBuffer = new CStructuredBuffer;
	m_Light2DBuffer->Create(sizeof(tLightInfo), 10, SB_TYPE::READ_ONLY, true);
}

void CRenderManager::render()
{
	//������ ���۽�, ȭ�� Clear
	float arrColor[4] = { 0.f, 0.f, 0.f, 1.f };
	CDevice::GetInst()->ClearTarget(arrColor);

	// ��� Ÿ�� ����(����Ÿ�� �ʱ�ȭ)   
	CDevice::GetInst()->OMSet();

	// ���� �� ���� ������ ������Ʈ �� ���ε�
	UpdateData();

	// ���� �Լ� ȣ��
	(this->*RENDER_FUNC)();

	// �������� ����
	Clear();
}

int CRenderManager::RegisterCamera(CCamera* _Cam, int _idx)
{
	// ī�޶� RenderManager ī�޶� ���� ���Ϳ� ����ϴ� �Լ�.

	// ����Ϸ��� �ε��� ��ȣ���� ���� ����� �۴ٸ� ���͸� �÷��ְ� ����Ѵ�.
	if (m_vecCam.size() <= _idx)
	{
		m_vecCam.resize(_idx + 1);
	}

	m_vecCam[_idx] = _Cam;
	return _idx;
}

void CRenderManager::SetRenderFunc(bool _IsPlay)
{
	// ���� ���� ���¿� ���� ������ �Լ� �б�ó��
	if (_IsPlay)
		RENDER_FUNC = &CRenderManager::render_play;
	else
		RENDER_FUNC = &CRenderManager::render_editor;
}

void CRenderManager::ClearCamera()
{
	m_vecCam.clear();
}

void CRenderManager::CopyRenderTarget()
{
	Ptr<CTexture> pRTTex = CDevice::GetInst()->GetRTTex();
	//Ptr<CTexture> pRTTex = CResourceManager::GetInst()->FindRes<CTexture>(L"RenderTargetTex");
	CONTEXT->CopyResource(m_RTCopyTex->GetTex2D().Get(), pRTTex->GetTex2D().Get());
}

void CRenderManager::UpdateData()
{
	// GlobalData �� ���� �������� ����
	GlobalData.Light2DCount = m_vecLight2D.size();

	// ����ȭ������ ũ�Ⱑ ���ڶ�� �� ũ�� ���� �����.
	if (m_Light2DBuffer->GetElementCount() < m_vecLight2D.size())
	{
		// ������ �ʰ� �� �ּҿ� ���� Create�Ͽ� �������. ���ο� nullptr �ʱ�ȭ�� ȣ��Ǿ� �� ���۴� �˾Ƽ� release�ȴ�.
		m_Light2DBuffer->Create((UINT)sizeof(tLightInfo), m_vecLight2D.size(),SB_TYPE::READ_ONLY,true);
	}

	// ����ȭ���۷� ���� �����͸� �ű��.
	m_Light2DBuffer->SetData(m_vecLight2D.data(), (UINT)sizeof(tLightInfo) * m_vecLight2D.size());
	m_Light2DBuffer->UpdateData(12, PIPELINE_STAGE::PS_PIXEL);		// ���������� PS�ܰ迡���� ���ȴ�

	// ���� ��� ������ ���ε�(����������, ��ǻƮ���̵� �ܰ�)
	CConstBuffer* pGlobalBuffer = CDevice::GetInst()->GetConstBuffer(CB_TYPE::GLOBAL);
	pGlobalBuffer->SetData(&GlobalData, sizeof(tGlobal));
	pGlobalBuffer->UpdateData();
	pGlobalBuffer->UpdateData_CS();	// ��ǻƮ���̵� �ܰ迡���� ��������� ���̹Ƿ�
}

void CRenderManager::render_play()
{
	// ī�޶� ���� ������
	for (size_t i = 0; i < m_vecCam.size(); ++i)
	{
		if (nullptr == m_vecCam[i])
			continue;

		m_vecCam[i]->SortObject();
		m_vecCam[i]->render();
	}
}

void CRenderManager::render_editor()
{
	m_pEditorCam->SortObject();
	m_pEditorCam->render();
}

void CRenderManager::Clear()
{
	m_vecLight2D.clear();
}
