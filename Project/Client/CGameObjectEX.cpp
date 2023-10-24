#include "pch.h"
#include "CGameObjectEX.h"

#include <Engine\CComponent.h>

CGameObjectEX::CGameObjectEX()
{
}

CGameObjectEX::~CGameObjectEX()
{
}

void CGameObjectEX::finaltick()
{
	//Component Type�� Script�� ��� Finaltick ��ü�� ȣ������ �ʴ´�
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::SCRIPT; i++)
	{ 
		if (nullptr != GetComponent((COMPONENT_TYPE)i))
			GetComponent((COMPONENT_TYPE)i)->finaltick();
	}

	// ������ ������ ������ �ڽ� ���͸� ���� ���ȣ���Ѵ�.
	const vector<CGameObject*>& vecChild = GetChild();
	for (size_t i = 0; i < vecChild.size(); ++i)
	{
		vecChild[i]->finaltick();
	}
}
