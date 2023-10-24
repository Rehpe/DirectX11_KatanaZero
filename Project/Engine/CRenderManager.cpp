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
	// 렌더타겟 해상도와 동일한 렌터타겟 복사용 텍스처 생성
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
	// 구조화버퍼의 경우 관리자가 없으므로 사용 시 잊지 말고 해제해주어야함
	if (nullptr != m_Light2DBuffer)
		delete m_Light2DBuffer;
}

void CRenderManager::init()
{
	// Light2DBuffer 구조화 버퍼 생성
	m_Light2DBuffer = new CStructuredBuffer;
	m_Light2DBuffer->Create(sizeof(tLightInfo), 10, SB_TYPE::READ_ONLY, true);
}

void CRenderManager::render()
{
	//렌더링 시작시, 화면 Clear
	float arrColor[4] = { 0.f, 0.f, 0.f, 1.f };
	CDevice::GetInst()->ClearTarget(arrColor);

	// 출력 타겟 지정(렌더타겟 초기화)   
	CDevice::GetInst()->OMSet();

	// 광원 및 전역 데이터 업데이트 및 바인딩
	UpdateData();

	// 렌더 함수 호출
	(this->*RENDER_FUNC)();

	// 광원벡터 비우기
	Clear();
}

int CRenderManager::RegisterCamera(CCamera* _Cam, int _idx)
{
	// 카메라를 RenderManager 카메라 관리 벡터에 등록하는 함수.

	// 등록하려는 인덱스 번호보다 벡터 사이즈가 작다면 벡터를 늘려주고 등록한다.
	if (m_vecCam.size() <= _idx)
	{
		m_vecCam.resize(_idx + 1);
	}

	m_vecCam[_idx] = _Cam;
	return _idx;
}

void CRenderManager::SetRenderFunc(bool _IsPlay)
{
	// 현재 레벨 상태에 따라 렌더링 함수 분기처리
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
	// GlobalData 에 광원 개수정보 세팅
	GlobalData.Light2DCount = m_vecLight2D.size();

	// 구조화버퍼의 크기가 모자라면 더 크게 새로 만든다.
	if (m_Light2DBuffer->GetElementCount() < m_vecLight2D.size())
	{
		// 지우지 않고 그 주소에 새로 Create하여 덧씌운다. 내부에 nullptr 초기화가 호출되어 전 버퍼는 알아서 release된다.
		m_Light2DBuffer->Create((UINT)sizeof(tLightInfo), m_vecLight2D.size(),SB_TYPE::READ_ONLY,true);
	}

	// 구조화버퍼로 광원 데이터를 옮긴다.
	m_Light2DBuffer->SetData(m_vecLight2D.data(), (UINT)sizeof(tLightInfo) * m_vecLight2D.size());
	m_Light2DBuffer->UpdateData(12, PIPELINE_STAGE::PS_PIXEL);		// 광원정보는 PS단계에서만 사용된다

	// 전역 상수 데이터 바인딩(파이프라인, 컴퓨트셰이드 단계)
	CConstBuffer* pGlobalBuffer = CDevice::GetInst()->GetConstBuffer(CB_TYPE::GLOBAL);
	pGlobalBuffer->SetData(&GlobalData, sizeof(tGlobal));
	pGlobalBuffer->UpdateData();
	pGlobalBuffer->UpdateData_CS();	// 컴퓨트셰이드 단계에서도 전역상수가 쓰이므로
}

void CRenderManager::render_play()
{
	// 카메라 기준 렌더링
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
