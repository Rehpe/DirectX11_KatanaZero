#include "pch.h"
#include "CState.h"

#include "CFSM.h"

CState::CState()
	: m_pOwnerFSM(nullptr)
{
}

CState::~CState()
{
}

void CState::finaltick()
{
}

void CState::Enter()
{
}

void CState::Exit()
{
}

CGameObject* CState::GetOwner()
{
	return m_pOwnerFSM->GetOwner();
}


