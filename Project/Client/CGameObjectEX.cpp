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
	//Component Type이 Script인 경우 Finaltick 자체를 호출하지 않는다
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::SCRIPT; i++)
	{ 
		if (nullptr != GetComponent((COMPONENT_TYPE)i))
			GetComponent((COMPONENT_TYPE)i)->finaltick();
	}

	// 본인의 연산이 끝나면 자식 벡터를 돌며 재귀호출한다.
	const vector<CGameObject*>& vecChild = GetChild();
	for (size_t i = 0; i < vecChild.size(); ++i)
	{
		vecChild[i]->finaltick();
	}
}
