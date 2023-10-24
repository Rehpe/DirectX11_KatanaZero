#include "pch.h"
#include "CAnimator2D.h"

#include "CAnim2D.h"
#include "CMeshRender.h"
#include "CParticleSystem.h"

#include "ptr.h"
#include "CMaterial.h"

#include "CResourceManager.h"
#include "CDevice.h"
#include "CEngine.h"

CAnimator2D::CAnimator2D()
	: CComponent(COMPONENT_TYPE::ANIMATOR2D)
	, m_pCurAnim(nullptr)
	, m_bRepeat(false)
	, m_bPause(false)
{
}

CAnimator2D::CAnimator2D(const CAnimator2D& _Other)
	: CComponent(_Other)
	, m_pCurAnim(_Other.m_pCurAnim)
	, m_bRepeat(_Other.m_bRepeat)
{
	//_other.m_mapAnim�� ��ü���� �ݺ����� ���� ���� �����Ͽ� ������ �ʿ� �־��ش�.
	map<wstring, CAnim2D*>::const_iterator iter = _Other.m_mapAnim.begin();
	for (; iter != _Other.m_mapAnim.end(); iter++)
	{
		//�ִϸ��̼� ������ Ŭ��
		CAnim2D* pAnim = iter->second->Clone();
		//�ִϸ��̼��� ���� �ִϸ����͸� �ڽ����� ����
		pAnim->m_pOwner = this;
		//�ִϸ��̼� ������ ���� ���� ����
		pAnim->Reset();
		//������ �ִϸ��̼� �ʿ� ���� ���� �ִϸ��̼� ����
		m_mapAnim.insert(make_pair(iter->first, pAnim));
	}

	//���簡 ������ ���� �ִϸ����Ϳ� ���� �ִϸ��̼��� �����Ų��
	if (m_pCurAnim != nullptr)
		Play(_Other.m_pCurAnim->GetName(), m_bRepeat);
}

CAnimator2D::~CAnimator2D()
{
	Safe_Del_Map(m_mapAnim);
}

void CAnimator2D::finaltick()
{
	if (nullptr != m_pCurAnim)
	{
		if (m_bPause)
			return;

		// �ݺ���� ���� && ���� �ִϸ��̼� ���Ῡ��
		if (m_bRepeat && m_pCurAnim->IsFinish())
		{
			m_pCurAnim->Reset();
		}

		m_pCurAnim->finaltick();
	}
}

void CAnimator2D::UpdateData()
{
	Ptr<CMaterial> pMtrl = nullptr;

	if (!ParticleSystem())
	{
		pMtrl = MeshRender()->GetMaterial();
	}
	else
	{
		pMtrl = ParticleSystem()->GetMaterial();
	}

	if (m_pCurAnim == nullptr)
		return;

	const tAnim2DFrm& frm = m_pCurAnim->GetCurFrame();
	Vec2 vBackSizeUV = m_pCurAnim->GetBackSizeUV();

	int iAnimUse = 1;

	// ������Ʈ�� ��ƼŬ�ý����� ������ �ִٸ� �ִϸ��̼� ��� ���θ� 1���� ���ε���
	if (ParticleSystem())
	{
		pMtrl->SetScalarParam(INT_1, &iAnimUse);
	}
	else
	{
		pMtrl->SetScalarParam(INT_0, &iAnimUse);
	}
	pMtrl->SetScalarParam(VEC2_0, &frm.LeftTopUV);
	pMtrl->SetScalarParam(VEC2_1, &frm.SliceUV);
	pMtrl->SetScalarParam(VEC2_2, &frm.OffsetUV);
	pMtrl->SetScalarParam(VEC2_3, &vBackSizeUV);

	pMtrl->SetTexParam(TEX_0, m_pCurAnim->GetAtlasTex());
	
}

void CAnimator2D::Clear()
{
	Ptr<CMaterial> pMtrl = MeshRender()->GetMaterial();

	int iAnimUse = 0;
	pMtrl->SetScalarParam(INT_0, &iAnimUse);

	//int iOverlayUse = 0;
	//pMtrl->SetScalarParam(INT_1, &iAnimUse);
	//
	//int iTransparentUse = 0;
	//pMtrl->SetScalarParam(INT_2, &iTransparentUse);

	Ptr<CTexture> pTex = nullptr;
	pMtrl->SetTexParam(TEX_0, pTex);
}

void CAnimator2D::CreateAnimation(const wstring& _strAnimName
	, Ptr<CTexture> _AtlasTex, Vec2 _vLeftTop, Vec2 _vSlice
	, Vec2 _vBackSize, int _FrameCount, int _FPS)
{
	CAnim2D* pAnim = new CAnim2D;
	pAnim->Create(_strAnimName, _AtlasTex, _vLeftTop, _vSlice, _vBackSize, _FrameCount, _FPS);

	pAnim->m_pOwner = this;
	m_mapAnim.insert(make_pair(_strAnimName, pAnim));
}

void CAnimator2D::Play(const wstring& _strName, bool _bRepeat)
{
	// �ִϸ��̼� �̸����� Find, ���ٸ� assert
	CAnim2D* pAnim = FindAnim(_strName);
	assert(pAnim);

	// ã�ƿ� �ִϸ��̼��� ���� �ִϸ��̼����� ����
	m_bPause = false;
	m_pCurAnim = pAnim;
	m_bRepeat = _bRepeat;
}

void CAnimator2D::Play(CAnim2D* _anim, bool _Repeat)
{
	// �ִϸ��̼��� ���� �ִϸ��̼����� ����
	m_bPause = false;
	m_pCurAnim = _anim;
	m_bRepeat = _Repeat;
}

void CAnimator2D::Pause()
{
	//if (m_bPause)
	//{
	//	m_bPause = false;
	//	return;
	//}

	m_bPause = true;
}


CAnim2D* CAnimator2D::FindAnim(const wstring& _strName)
{
	map<wstring, CAnim2D*>::iterator iter = m_mapAnim.find(_strName);

	if (iter == m_mapAnim.end())
	{
		return nullptr;
	}

	return iter->second;
}

void CAnimator2D::InsertAnim(CAnim2D* _CurAnim)
{
	_CurAnim->m_pOwner = this;
	m_mapAnim.insert(make_pair(_CurAnim->GetName(), _CurAnim)); 
	SetCurAnim(_CurAnim);
}

void CAnimator2D::SetCurAnim(CAnim2D* _CurAnim)
{
	// ã�ƿ� �ִϸ��̼��� ���� �ִϸ��̼����� ����
	m_pCurAnim = _CurAnim;
}

void CAnimator2D::RemoveAnim(const wstring _AnimName)
{
	m_mapAnim.erase(_AnimName);
}

void CAnimator2D::SaveToLevelFile(FILE* _File)
{
	// �ִϸ��̼� �ݺ���� ����
	fwrite(&m_bRepeat, sizeof(bool), 1, _File);

	// �ִϸ��̼� ��� ����
	size_t AnimCount = m_mapAnim.size();
	fwrite(&AnimCount, sizeof(size_t), 1, _File);

	// �� �ִϸ��̼� �̸�, ��ΰ� ����
	for (const auto& pair : m_mapAnim)
	{
		// �ִϸ��̼� �̸� ����
		SaveWString(pair.first, _File);

		// �ִϸ��̼� ��� ����
		wstring RelativePath = (pair.second)->GetRelativePath();
		SaveWString(RelativePath, _File);
	}

	// ���� ��� ���� �ִϸ��̼�
	wstring strCurAnimName;
	if (nullptr != m_pCurAnim)
	{
		strCurAnimName = m_pCurAnim->GetName();
	}
	SaveWString(strCurAnimName, _File);
}

void CAnimator2D::LoadFromLevelFile(FILE* _File)
{
	// �ִϸ��̼� �ݺ���� ����
	fread(&m_bRepeat, sizeof(bool), 1, _File);

	// �ִϸ��̼� ��� ����
	size_t AnimCount = 0;
	fread(&AnimCount, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < AnimCount; ++i)
	{
		// �� �ִϸ��̼� ����
		CAnim2D* pNewAnim = new CAnim2D;
		
		// �ִϸ��̼� �̸� ����
		wstring AnimName;
		LoadWString(AnimName, _File);
		pNewAnim->SetName(AnimName);

		// ��ηκ��� �ش� �ִϸ��̼� ��Ʋ�� �� ������ ���� �ҷ�����
		wstring RelativePath;
		LoadWString(RelativePath, _File);
		pNewAnim->Load(RelativePath);

		// �ִϸ��̼� �ʿ� ���
		m_mapAnim.insert(make_pair(pNewAnim->GetName(), pNewAnim));
		pNewAnim->m_pOwner = this;
	}

	// ���� ��� ���� �ִϸ��̼�
	wstring strCurAnimName;
	LoadWString(strCurAnimName, _File);
	m_pCurAnim = FindAnim(strCurAnimName);
}
