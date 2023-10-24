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
	// �⺻������ ���� ������ ����
	m_pSharedMtrl = _Mtrl;
	m_pCurrentMtrl = m_pSharedMtrl;
}

Ptr<CMaterial> CRenderComponent::GetDynamicMaterial()
{
	// ���� ����(��������)�� ���� -> Nullptr ��ȯ
	if (nullptr == m_pSharedMtrl)
	{
		m_pCurrentMtrl = nullptr;
		return m_pCurrentMtrl;
	}

	// ���� ���� ���� ��û�� �����ؼ�(������ �����Ͽ�) �ش�.
	if (nullptr == m_pDynamicMtrl)
	{
		m_pDynamicMtrl = m_pSharedMtrl->Clone();
	}

	// �������� ���� ������ �ִٸ� �� �ּҸ� ���� ��������� �����Ѵ�.
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
