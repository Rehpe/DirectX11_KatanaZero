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
	//_other.m_mapAnim의 객체들을 반복문을 돌며 깊은 복사하여 복사할 맵에 넣어준다.
	map<wstring, CAnim2D*>::const_iterator iter = _Other.m_mapAnim.begin();
	for (; iter != _Other.m_mapAnim.end(); iter++)
	{
		//애니메이션 포인터 클론
		CAnim2D* pAnim = iter->second->Clone();
		//애니메이션의 소유 애니메이터를 자신으로 설정
		pAnim->m_pOwner = this;
		//애니메이션 프레임 정보 등을 리셋
		pAnim->Reset();
		//복사한 애니메이션 맵에 새로 만든 애니메이션 삽입
		m_mapAnim.insert(make_pair(iter->first, pAnim));
	}

	//복사가 끝나면 원본 애니메이터와 같은 애니메이션을 재생시킨다
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

		// 반복재생 여부 && 현재 애니메이션 종료여부
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

	// 오브젝트가 파티클시스템을 가지고 있다면 애니메이션 사용 여부를 1번에 바인딩함
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
	// 애니메이션 이름으로 Find, 없다면 assert
	CAnim2D* pAnim = FindAnim(_strName);
	assert(pAnim);

	// 찾아온 애니메이션을 현재 애니메이션으로 설정
	m_bPause = false;
	m_pCurAnim = pAnim;
	m_bRepeat = _bRepeat;
}

void CAnimator2D::Play(CAnim2D* _anim, bool _Repeat)
{
	// 애니메이션을 현재 애니메이션으로 설정
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
	// 찾아온 애니메이션을 현재 애니메이션으로 설정
	m_pCurAnim = _CurAnim;
}

void CAnimator2D::RemoveAnim(const wstring _AnimName)
{
	m_mapAnim.erase(_AnimName);
}

void CAnimator2D::SaveToLevelFile(FILE* _File)
{
	// 애니메이션 반복재생 여부
	fwrite(&m_bRepeat, sizeof(bool), 1, _File);

	// 애니메이션 목록 개수
	size_t AnimCount = m_mapAnim.size();
	fwrite(&AnimCount, sizeof(size_t), 1, _File);

	// 각 애니메이션 이름, 경로값 저장
	for (const auto& pair : m_mapAnim)
	{
		// 애니메이션 이름 저장
		SaveWString(pair.first, _File);

		// 애니메이션 경로 저장
		wstring RelativePath = (pair.second)->GetRelativePath();
		SaveWString(RelativePath, _File);
	}

	// 현재 재생 중인 애니메이션
	wstring strCurAnimName;
	if (nullptr != m_pCurAnim)
	{
		strCurAnimName = m_pCurAnim->GetName();
	}
	SaveWString(strCurAnimName, _File);
}

void CAnimator2D::LoadFromLevelFile(FILE* _File)
{
	// 애니메이션 반복재생 여부
	fread(&m_bRepeat, sizeof(bool), 1, _File);

	// 애니메이션 목록 개수
	size_t AnimCount = 0;
	fread(&AnimCount, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < AnimCount; ++i)
	{
		// 새 애니메이션 생성
		CAnim2D* pNewAnim = new CAnim2D;
		
		// 애니메이션 이름 설정
		wstring AnimName;
		LoadWString(AnimName, _File);
		pNewAnim->SetName(AnimName);

		// 경로로부터 해당 애니메이션 아틀라스 및 프레임 정보 불러오기
		wstring RelativePath;
		LoadWString(RelativePath, _File);
		pNewAnim->Load(RelativePath);

		// 애니메이션 맵에 등록
		m_mapAnim.insert(make_pair(pNewAnim->GetName(), pNewAnim));
		pNewAnim->m_pOwner = this;
	}

	// 현재 재생 중인 애니메이션
	wstring strCurAnimName;
	LoadWString(strCurAnimName, _File);
	m_pCurAnim = FindAnim(strCurAnimName);
}
