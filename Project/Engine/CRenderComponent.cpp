#include "pch.h"
#include "CRenderComponent.h"

CRenderComponent::CRenderComponent(COMPONENT_TYPE _type)
	:CComponent(_type)
{
}

CRenderComponent::~CRenderComponent()
{
}

void CRenderComponent::SetMaterial(Ptr<CMaterial> _Mtrl)
{
	// 기본적으로 공유 재질로 세팅
	m_pSharedMtrl = _Mtrl;
	m_pCurrentMtrl = m_pSharedMtrl;
}

Ptr<CMaterial> CRenderComponent::GetDynamicMaterial()
{
	// 원본 재질(공유재질)이 없다 -> Nullptr 반환
	if (nullptr == m_pSharedMtrl)
	{
		m_pCurrentMtrl = nullptr;
		return m_pCurrentMtrl;
	}

	// 동적 재질 최초 요청시 제작해서(원본을 복사하여) 준다.
	if (nullptr == m_pDynamicMtrl)
	{
		m_pDynamicMtrl = m_pSharedMtrl->Clone();
	}

	// 만들어놓은 동적 재질이 있다면 그 주소를 현재 사용재질로 지정한다.
	m_pCurrentMtrl = m_pDynamicMtrl;


	return m_pCurrentMtrl;
}

void CRenderComponent::SaveToLevelFile(FILE* _File)
{
	SaveResourceRef(m_pMesh.Get(), _File);
	SaveResourceRef(m_pSharedMtrl.Get(), _File);
}

void CRenderComponent::LoadFromLevelFile(FILE* _File)
{
	LoadResourceRef(m_pMesh, _File);
	LoadResourceRef(m_pSharedMtrl, _File);

	SetMaterial(m_pSharedMtrl);
}
