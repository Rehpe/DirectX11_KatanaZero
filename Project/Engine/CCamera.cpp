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
	, m_iLayerMask(0)			// �ʱ� ����: �ƹ� ���̾ ������ ����
{
	SetName(L"Camera");

	Vec2 RenderResoluion = CDevice::GetInst()->GetRenderResolution();
	m_fAspectRatio = RenderResoluion.x / RenderResoluion.y;			   // ��Ⱦ�� ����
}

CCamera::CCamera(const CCamera& _Other)		// ��� ����, ������ �з����ʹ� finaltick���� �� ������ ���� ���ǹǷ� �ʿ����
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
	CalcViewMat();		// �� ��� ���
	CalcProjMat();		// ���� ��� ���
}

void CCamera::CalcViewMat()
{
	// ===== View Matrix (�� ���, Ȥ�� ī�޶� ���) ��� ======

	// View ����� ī�޶� �ٶ󺸴� ������ z���� ��ķ�,
	// WinApi�� �������� ī�޶� �̵��� ��ŭ �ٶ󺸴� ������Ʈ�� ��ġ���� ����� ��ó��,
	// ī�޶� �ٶ󺸴� ������ �����̶�� �����ϰ� �������� ī�޶��� ��ġ��ŭ�� ������Ʈ�� ��ġ���� ���־�
	// View����� �̵� ����� ���Ѵ�. (_41, _42, _43���� ���� x,y,z ���� ��ġ���� ����)

	m_matView = XMMatrixIdentity();		// �������

	// ī�޶��� ���� ��ġ�� �������� ���̰���ŭ�� ���� ���� View �̵���� (ī�޶� ��ǥ�� �������� �̵�)
	Vec3 vCamPos = Transform()->GetRelativePos();
	Matrix matViewTrans = XMMatrixTranslation(-vCamPos.x, -vCamPos.y, -vCamPos.z);

	// 2D������ ��� ī�޶� ȸ������ �ʱ� ������ ȸ������� �������ó���Ѵ�.
	// ���� 3D������ ī�޶� ȸ���� �ִ� ��� World��� ���ϴ� �Ͱ��� �ݴ��, �̵��� ���� ��Ų �� ȸ������� ���ؾ��Ѵ�

	// ī�޶� �ٶ󺸴� ������ z���̶� �������� ���� ī�޶��� �ະ ȸ��
	// x�� ȸ��: ���� �� �Ʒ��� �����̴� ȸ�� (Pitch)
	// y�� ȸ��: ���� �¿�� ���������ϴ� ȸ�� (Yaw)
	// z�� ȸ��: ���� ����װŸ��� ȸ�� (Roll)

	// ī�޶� �ٶ󺸴� ������ z��� �����ϰ� ����� ȸ�� ��� ���� (View ȸ�� ���)
	Matrix matViewRot = XMMatrixIdentity();

	// Transform���� ī�޶��� ȸ������(�������, ���� ����)�� ������´�
	Vec3 vR = Transform()->GetWorldDir(DIR_TYPE::RIGHT);
	Vec3 vU = Transform()->GetWorldDir(DIR_TYPE::UP);
	Vec3 vF = Transform()->GetWorldDir(DIR_TYPE::FRONT);

	// �츮�� ���ϰ��� �ϴ� ����, ȸ���Ǿ� �ִ� ī�޶� �������� �������� z�� ������ �ٶ󺸰� �ϴ� ��.
	// ī�޶� z�� ������ �ٶ󺸰� ����ٴ� ���� DefaultDir, �� ������ķ� ����ٴ� �� ( 100, 010, 001 )
	// ī�޶��� ���� ������Ŀ� ��� ȸ�� ��� X�� ���� ī�޶� ���� ������ ������ķ� �����,
	// ī�޶� �ٶ󺸴� ������Ʈ�鿡�Ե� �Ȱ��� ȸ�� ��� X�� �����ָ� �Ǵ� ���̴�.

	// ��������� �밢���� 1�̰� �������� 0�� 3x3 ������İ� ������, �̸� Ǯ� �ؼ��ϸ�
	// ī�޶��� ���� ������� A�� ���ؾ� �� ȸ�� ��� X�� ������ �� ��������� ��ȯ�ϴ�, �� A ����� ������̶�� �Ҹ��̴�.

	// ��������� ��� x,y,z ���� ���� ����(���� 90��)�ϴ� ��������, ������� ���ϸ�
	// A�� (Rx Ry Rz	��� ����� A', �� ���ϰ��� �ϴ� X��		 (Rx Ux Fx		�̴�. (�밢���� �������� �ڸ��� �ٲٴ� ��ġ)
	//		Ux Uy Uz										  Ry Uy Fy
	//		Fx Fy Fz)										  Rz Uz Fz)

	// �� �� ����� ���ϸ� ��� ��������� ������ �Ǵ°ɱ�? ����� �����̴�.
	// �����̶� ����(����)x����(����)xcos�� 
	// �� ��Ȳ���� ���ʹ� Normalize�� ���⺤���̹Ƿ� 1�̹Ƿ� ������ cos�迡 ���� ������� �ٲ� ���̴�.
	// ��������� ��� x,y,z���� ���� ����(90��)�Ͽ� ������ �����Ƿ�, ���γ���(x�� x����, y�� y�ೢ��..) ���ϴ� ���� �ƴ� �̻�
	// cos90 = 0�̹Ƿ� ���� 0�� ���� ���̴�. ��, ������ ��� ��������� ������ �ȴ�.

	matViewRot._11 = vR.x;	matViewRot._12 = vU.x;	matViewRot._13 = vF.x;
	matViewRot._21 = vR.y;	matViewRot._22 = vU.y;	matViewRot._23 = vF.y;
	matViewRot._31 = vR.z;	matViewRot._32 = vU.z;	matViewRot._33 = vF.z;

	m_matView = matViewTrans * matViewRot;
}

void CCamera::CalcProjMat()
{
	// ===== ���� ��� (Projection Matrix) ��� =====
	m_matProj = XMMatrixIdentity();

	if (PROJ_TYPE::ORTHOGRAPHIC == m_ProjType)
	{
		// =============
		// ���� ���� (���ٰ� ���� �״�� ���������� ��ܼ� ��� ������, ���� �ػ󵵿� �����ػ󵵰� 1:1�� ����)
		// ==============

		// 3D�������� 2D������ ������ �� ���� ����

		// �ػ󵵸�ŭ ������ ~1 ~ 1�� NDC ��ǥ�迡 �����Ͽ� ���� �� �ֵ��� ����ȭ���ش�
		Vec2 vResolution = CDevice::GetInst()->GetRenderResolution();

		// ���� ���������� ȭ�� Ȯ�� / ����� �ǹ�?
		// ���� ������ �ѹ��� ���� ȭ���� ���� ���̳�, �ƴϸ� �� ���� ȭ���� ���� ���̳��� ����
		// ���� ȭ���� ���� ���, ��ü���� �۾��� ���̰�, ���� ȭ���� ���� ��� �׸�ŭ ��ü�� ȭ�鿡�� �����ϴ� ������ Ŀ���� Ŀ�� ��
		// ���� ȭ�� Ȯ�븦 2�� �ϴ� ����, 2��� ���� ������ ��� �Ͱ� ���� �ǹ�
		// ��������� Ȯ�� ���� m_fScale�� ������ ���� ���� �� �ػ� x, y ���� �����־�� ���ϴ� ��� Ȯ��/��Ұ� �����ϴ�

		m_matProj = XMMatrixOrthographicLH(vResolution.x * (1.f / m_fScale)
			, vResolution.y * (1.f / m_fScale)
			, 1.f
			, 10000.f);
	}
	else
	{
		// ======================
		// ���� ���� (�Ÿ��� ���� ���ٰ��� ���� ) 
		// ======================

		// �þ߰�, ��Ⱦ��, Near, Far
		m_matProj = XMMatrixPerspectiveFovLH(XM_PI / 2.f, m_fAspectRatio, 1.f, 10000.f);

		// �����ﰢ���� �ﰢ�� �̿��Ͽ� �����Ǵ� ũ�⸦ �����ϴµ�, ������Ʈ�� z��ġ�� ���� ������ ����ϹǷ�
		// ��Ȯ�� ������ǥ�� ���ϴ� �ϳ��� ����� ������ �� ����
		// ���� ������ view����� z���� x,y,z�� ������ �ִ� ���·� ����� ��ȯ�ǹǷ� ���������δ� ������ w��( = view ����� z)
		// ���� ������� ��Ȯ�� ���� ������ ��İ��� ���´�
		// �� �۾��� Rasterize �ܰ迡�� �˾Ƽ� ���ֱ� ������, �츮�� ���� �����ִ� �۾��� ���� �ʿ�� ����
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
		m_iLayerMask |= 1 << _iLayer;		// Visible �ɼ��� ��� ���̾� �ڸ��� 1 ����
	else
		m_iLayerMask &= ~(1 << _iLayer);	// Non Visible �ɼ��� ��� ���̾� �ڸ��� ��Ʈ ������
}

void CCamera::SetLayerMaskAll(bool _Visible)
{
	// ���̾� ���θ� Visible/Non Visible�� �����ϴ� �Լ�
	if (_Visible)
		m_iLayerMask = 0xffffffff;
	else
		m_iLayerMask = 0;
}


void CCamera::SortObject()
{
	// ������Ʈ �� ��� ������ �������� ��� �����Ǿ��ֳĿ� ���� ������ ������ ���� ��
	
	// ������Ʈ �з� ��, ���� �������� �з� ������ ����ִ� ���͸� ���� clear���ش�.
	clear();

	// �����Ŵ������� ���� ���� �޾ƿͼ� �з�
	CLevel* pCurLevel = CLevelManager::GetInst()->GetCurLevel();

	for (UINT i = 0; i < MAX_LAYER; i++)
	{
		// ���̾� ����ũ Ȯ��
		if (m_iLayerMask & (1 << i))	// ���̾� ����ũ�� i��° �ڸ� ���� 1 (��´�) �� ���
		{
			// ������ �ش� ���̾ ������ ���̾ ���� '��� ������Ʈ ����'�� �ҷ��´� (�θ��� �ƴ�!)
			CLayer* pLayer = pCurLevel->GetLayer(i);
			const vector<CGameObject*>& vecObject = pLayer->GetObjects();

			for (size_t j = 0; j < vecObject.size(); j++)
			{
				// ������Ʈ�� ���� ������Ʈ ���θ� Ȯ���Ѵ�
				CRenderComponent* pRenderComp = vecObject[j]->GetRenderComponent();

				if (nullptr == pRenderComp									// RenderComp�� ���ų�
					|| nullptr == pRenderComp->GetMaterial()				// ������ ������ ���ų�
					|| nullptr == pRenderComp->GetMaterial()->GetShader())	// ������ Shader �� �ϳ��� ���ٸ�
					continue;												// ������ ����

				// Shader Domain�� ���� �з�
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
	// ī�޶� ���� �밡 ���� �� �ֱ� ������, ������ ī�޶󿡼� �������ϱ� ���� ����� ������Ʈ�Ѵ�.
	g_transform.matView = m_matView;
	g_transform.matProj = m_matProj;

	// ���̴� �����ο� ���� ���������� �׸���
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
	// ������ �з� ���ʹ� �� finaltick���� �з��ǹǷ� ������ �ʿ� ����
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
