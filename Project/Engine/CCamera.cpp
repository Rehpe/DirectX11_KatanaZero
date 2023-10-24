#include "pch.h"
#include "CCamera.h"

#include "CTransform.h"
#include "CDevice.h"
#include "CRenderManager.h"

#include "CLevelManager.h"
#include "CLevel.h"
#include "CLayer.h"
#include "CRenderComponent.h"

CCamera::CCamera()
	:CComponent(COMPONENT_TYPE::CAMERA)
	, m_fAspectRatio(1.f)
	, m_fScale(1.f)
	, m_ProjType(PROJ_TYPE::ORTHOGRAPHIC)
	, m_iCamIdx(-1)
	, m_iLayerMask(0)			// 초기 세팅: 아무 레이어도 보이지 않음
{
	SetName(L"Camera");

	Vec2 RenderResoluion = CDevice::GetInst()->GetRenderResolution();
	m_fAspectRatio = RenderResoluion.x / RenderResoluion.y;			   // 종횡비 세팅
}

CCamera::CCamera(const CCamera& _Other)		// 행렬 정보, 도메인 분류벡터는 finaltick에서 매 프레임 새로 계산되므로 필요없음
	: CComponent(_Other)
	, m_fAspectRatio(_Other.m_fAspectRatio)
	, m_fScale(_Other.m_fScale)
	, m_ProjType(_Other.m_ProjType)
	, m_iLayerMask(_Other.m_iLayerMask)
	, m_iCamIdx(-1)
{
}

CCamera::~CCamera()
{
}

void CCamera::begin()
{
	if (-1 != m_iCamIdx)
	{
		CRenderManager::GetInst()->RegisterCamera(this, m_iCamIdx);
	}
}

void CCamera::finaltick()
{
	CalcViewMat();		// 뷰 행렬 계산
	CalcProjMat();		// 투영 행렬 계산
}

void CCamera::CalcViewMat()
{
	// ===== View Matrix (뷰 행렬, 혹은 카메라 행렬) 계산 ======

	// View 행렬은 카메라가 바라보는 방향이 z축인 행렬로,
	// WinApi때 원점에서 카메라가 이동한 만큼 바라보는 오브젝트의 위치값을 빼줬던 것처럼,
	// 카메라가 바라보는 방향을 원점이라고 가정하고 원점에서 카메라의 위치만큼을 오브젝트의 위치에서 빼주어
	// View행렬의 이동 행렬을 구한다. (_41, _42, _43열에 각각 x,y,z 축의 위치값이 들어간다)

	m_matView = XMMatrixIdentity();		// 단위행렬

	// 카메라의 현재 위치와 원점과의 차이값만큼을 빼준 것이 View 이동행렬 (카메라 좌표를 원점으로 이동)
	Vec3 vCamPos = Transform()->GetRelativePos();
	Matrix matViewTrans = XMMatrixTranslation(-vCamPos.x, -vCamPos.y, -vCamPos.z);

	// 2D게임의 경우 카메라가 회전하지 않기 때문에 회전행렬은 단위행렬처리한다.
	// 만일 3D게임의 카메라 회전이 있는 경우 World행렬 구하는 것과는 반대로, 이동을 먼저 시킨 후 회전행렬을 곱해야한다

	// 카메라가 바라보는 방향이 z축이라 가정했을 때의 카메라의 축별 회전
	// x축 회전: 고개를 위 아래로 끄덕이는 회전 (Pitch)
	// y축 회전: 고개를 좌우로 도리도리하는 회전 (Yaw)
	// z축 회전: 고개를 갸우뚱거리는 회전 (Roll)

	// 카메라가 바라보는 방향을 z축과 평행하게 만드는 회전 행렬 적용 (View 회전 행렬)
	Matrix matViewRot = XMMatrixIdentity();

	// Transform에서 카메라의 회전방향(월드방향, 최종 방향)을 가지고온다
	Vec3 vR = Transform()->GetWorldDir(DIR_TYPE::RIGHT);
	Vec3 vU = Transform()->GetWorldDir(DIR_TYPE::UP);
	Vec3 vF = Transform()->GetWorldDir(DIR_TYPE::FRONT);

	// 우리가 구하고자 하는 것은, 회전되어 있는 카메라를 원점으로 돌려놓고 z축 방향을 바라보게 하는 것.
	// 카메라가 z축 방향을 바라보게 만든다는 것은 DefaultDir, 즉 기저행렬로 만든다는 것 ( 100, 010, 001 )
	// 카메라의 현재 방향행렬에 어떠한 회전 행렬 X를 곱해 카메라가 보는 방향을 기저행렬로 만들고,
	// 카메라가 바라보는 오브젝트들에게도 똑같이 회전 행렬 X를 곱해주면 되는 것이다.

	// 기저행렬은 대각선이 1이고 나머지가 0인 3x3 단위행렬과 같은데, 이를 풀어서 해석하면
	// 카메라의 현재 방향행렬 A에 곱해야 할 회전 행렬 X는 곱했을 때 단위행렬을 반환하는, 즉 A 행렬의 역행렬이라는 소리이다.

	// 방향행렬의 경우 x,y,z 축이 전부 직교(서로 90도)하는 선분으로, 역행렬을 구하면
	// A가 (Rx Ry Rz	라면 역행렬 A', 즉 구하고자 하는 X는		 (Rx Ux Fx		이다. (대각선을 기준으로 자리를 바꾸는 전치)
	//		Ux Uy Uz										  Ry Uy Fy
	//		Fx Fy Fz)										  Rz Uz Fz)

	// 이 두 행렬을 곱하면 어떻게 단위행렬이 나오게 되는걸까? 행렬의 내적이다.
	// 내적이란 길이(벡터)x길이(벡터)xcosθ 
	// 이 상황에서 벡터는 Normalize된 방향벡터이므로 1이므로 마지막 cosθ에 따라 결과값이 바뀔 것이다.
	// 방향행렬의 경우 x,y,z축이 전부 직교(90도)하여 만나지 않으므로, 본인끼리(x는 x끼리, y는 y축끼리..) 곱하는 것이 아닌 이상
	// cos90 = 0이므로 전부 0이 나올 것이다. 즉, 곱했을 경우 단위행렬이 나오게 된다.

	matViewRot._11 = vR.x;	matViewRot._12 = vU.x;	matViewRot._13 = vF.x;
	matViewRot._21 = vR.y;	matViewRot._22 = vU.y;	matViewRot._23 = vF.y;
	matViewRot._31 = vR.z;	matViewRot._32 = vU.z;	matViewRot._33 = vF.z;

	m_matView = matViewTrans * matViewRot;
}

void CCamera::CalcProjMat()
{
	// ===== 투영 행렬 (Projection Matrix) 계산 =====
	m_matProj = XMMatrixIdentity();

	if (PROJ_TYPE::ORTHOGRAPHIC == m_ProjType)
	{
		// =============
		// 직교 투영 (원근감 없이 그대로 일직선으로 당겨서 찍는 투영법, 월드 해상도와 렌더해상도가 1:1로 동일)
		// ==============

		// 3D공간에서 2D게임을 제작할 때 많이 쓰임

		// 해상도만큼 나누어 ~1 ~ 1인 NDC 좌표계에 압축하여 찍을 수 있도록 정규화해준다
		Vec2 vResolution = CDevice::GetInst()->GetRenderResolution();

		// 직교 투영에서의 화면 확대 / 축소의 의미?
		// 직교 투영은 한번에 많은 화면을 담을 것이냐, 아니면 더 적은 화면을 담을 것이냐의 관점
		// 많은 화면을 담을 경우, 물체들이 작아질 것이고, 작은 화면을 담을 경우 그만큼 물체가 화면에서 차지하는 비율이 커지며 커질 것
		// 따라서 화면 확대를 2배 하는 것은, 2배로 좁은 영역을 찍는 것과 같은 의미
		// 결과적으로 확대 비율 m_fScale을 역수로 취한 값을 각 해상도 x, y 값에 곱해주어야 원하는 대로 확대/축소가 가능하다

		m_matProj = XMMatrixOrthographicLH(vResolution.x * (1.f / m_fScale)
			, vResolution.y * (1.f / m_fScale)
			, 1.f
			, 10000.f);
	}
	else
	{
		// ======================
		// 원근 투영 (거리에 따라 원근감이 생김 ) 
		// ======================

		// 시야각, 종횡비, Near, Far
		m_matProj = XMMatrixPerspectiveFovLH(XM_PI / 2.f, m_fAspectRatio, 1.f, 10000.f);

		// 직각삼각형의 삼각비를 이용하여 투영되는 크기를 결정하는데, 오브젝트의 z위치에 따라 비율로 계산하므로
		// 정확한 투영좌표를 구하는 하나의 행렬이 존재할 수 없음
		// 따라서 본인의 view행렬의 z값이 x,y,z에 곱해져 있는 상태로 행렬이 반환되므로 최종적으로는 본인의 w값( = view 행렬의 z)
		// 으로 나눠줘야 정확한 원근 투영의 행렬값이 나온다
		// 이 작업을 Rasterize 단계에서 알아서 해주기 때문에, 우리가 따로 나눠주는 작업을 해줄 필요는 없다
	}

}


void CCamera::SetCameraIndex(int _idx)
{
	m_iCamIdx = _idx;
	CRenderManager::GetInst()->RegisterCamera(this, m_iCamIdx);
}

void CCamera::SetLayerMask(int _iLayer, bool _Visible)
{
	if (_Visible)
		m_iLayerMask |= 1 << _iLayer;		// Visible 옵션일 경우 레이어 자리에 1 삽입
	else
		m_iLayerMask &= ~(1 << _iLayer);	// Non Visible 옵션일 경우 레이어 자리에 비트 빼버림
}

void CCamera::SetLayerMaskAll(bool _Visible)
{
	// 레이어 전부를 Visible/Non Visible로 설정하는 함수
	if (_Visible)
		m_iLayerMask = 0xffffffff;
	else
		m_iLayerMask = 0;
}


void CCamera::SortObject()
{
	// 오브젝트 별 사용 재질에 도메인이 어떻게 설정되어있냐에 따라 렌더링 순서를 잡을 것
	
	// 오브젝트 분류 전, 이전 프레임의 분류 정보가 들어있는 벡터를 전부 clear해준다.
	clear();

	// 레벨매니저에게 현재 레벨 받아와서 분류
	CLevel* pCurLevel = CLevelManager::GetInst()->GetCurLevel();

	for (UINT i = 0; i < MAX_LAYER; i++)
	{
		// 레이어 마스크 확인
		if (m_iLayerMask & (1 << i))	// 레이어 마스크의 i번째 자리 값이 1 (찍는다) 인 경우
		{
			// 레벨의 해당 레이어를 가져와 레이어가 가진 '모든 오브젝트 벡터'를 불러온다 (부모벡터 아님!)
			CLayer* pLayer = pCurLevel->GetLayer(i);
			const vector<CGameObject*>& vecObject = pLayer->GetObjects();

			for (size_t j = 0; j < vecObject.size(); j++)
			{
				// 오브젝트의 렌더 컴포넌트 여부를 확인한다
				CRenderComponent* pRenderComp = vecObject[j]->GetRenderComponent();

				if (nullptr == pRenderComp									// RenderComp가 없거나
					|| nullptr == pRenderComp->GetMaterial()				// 설정된 재질이 없거나
					|| nullptr == pRenderComp->GetMaterial()->GetShader())	// 설정된 Shader 중 하나라도 없다면
					continue;												// 렌더링 제외

				// Shader Domain에 따른 분류
				SHADER_DOMAIN eDomain = pRenderComp->GetMaterial()->GetShader()->GetDomain();
				switch (eDomain)
				{
				case SHADER_DOMAIN::DOMAIN_OPAQUE:
					m_vecOpaque.push_back(vecObject[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_MASK:
					m_vecMask.push_back(vecObject[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_TRANSPARENT:
					m_vecTransparent.push_back(vecObject[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_POSTPROCESS:
					m_vecPost.push_back(vecObject[j]);
					break;
				case SHADER_DOMAIN::DOMAIN_UI:
					m_vecUI.push_back(vecObject[j]);
					break;
				}
			}
		}
	}
}

void CCamera::render()
{
	// 카메라가 여러 대가 있을 수 있기 때문에, 각각의 카메라에서 렌더링하기 직전 행렬을 업데이트한다.
	g_transform.matView = m_matView;
	g_transform.matProj = m_matProj;

	// 쉐이더 도메인에 따라서 순차적으로 그리기
	render_opaque();
	render_mask();
	render_transparent();

	// Post Process
	render_postprocess();

	// UI
	render_ui();
}

void CCamera::clear()
{
	m_vecOpaque.clear();
	m_vecMask.clear();
	m_vecTransparent.clear();
	m_vecPost.clear();
	m_vecUI.clear();
}

void CCamera::render_opaque()
{
	for (size_t i = 0; i < m_vecOpaque.size(); ++i)
	{
		m_vecOpaque[i]->render();
	}
}

void CCamera::render_mask()
{
	for (size_t i = 0; i < m_vecMask.size(); ++i)
	{
		m_vecMask[i]->render();
	}
}

void CCamera::render_transparent()
{
	for (size_t i = 0; i < m_vecTransparent.size(); ++i)
	{
		m_vecTransparent[i]->render();
	}
}

void CCamera::render_postprocess()
{
	for (size_t i = 0; i < m_vecPost.size(); ++i)
	{
		CRenderManager::GetInst()->CopyRenderTarget();
		m_vecPost[i]->render();
	}
}

void CCamera::render_ui()
{
	for (size_t i = 0; i < m_vecUI.size(); ++i)
	{
		m_vecUI[i]->render();
	}
}

void CCamera::SaveToLevelFile(FILE* _File)
{
	// 도메인 분류 벡터는 매 finaltick마다 분류되므로 저장할 필요 없음
	fwrite(&m_fAspectRatio, sizeof(float), 1, _File);
	fwrite(&m_fScale, sizeof(float), 1, _File);
	fwrite(&m_ProjType, sizeof(UINT), 1, _File);
	fwrite(&m_iLayerMask, sizeof(UINT), 1, _File);
	fwrite(&m_iCamIdx, sizeof(int), 1, _File);
}

void CCamera::LoadFromLevelFile(FILE* _File)
{
	fread(&m_fAspectRatio, sizeof(float), 1, _File);
	fread(&m_fScale, sizeof(float), 1, _File);
	fread(&m_ProjType, sizeof(UINT), 1, _File);
	fread(&m_iLayerMask, sizeof(UINT), 1, _File);
	fread(&m_iCamIdx, sizeof(int), 1, _File);
}
