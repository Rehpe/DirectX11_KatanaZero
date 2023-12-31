#include "pch.h"
#include "CFSM.h"
#include "CState.h"

CFSM::CFSM()
	: CComponent(COMPONENT_TYPE::FSM_AI)
	, m_pCurState(nullptr)
{
}

CFSM::CFSM(const CFSM& _other)
	:CComponent(_other)
	, m_pCurState(_other.m_pCurState)
{
}

CFSM::~CFSM()
{
	Safe_Del_Map(m_mapState);
}

void CFSM::finaltick()
{
	// FSM은 본인이 가진 State를 관리하고, 실행만 시켜준다
	if (m_pCurState == nullptr)
		return;

	m_pCurState->finaltick();
}

void CFSM::AddState(const wstring& _strName, CState* _pState)
{
	// State 맵에 해당 State를 추가한다
	m_mapState.insert(make_pair(_strName, _pState));

	// 해당 state의 소유 FSM을 본인으로 설정한다.
	_pState->m_pOwnerFSM = this;

	// 해당 State의 Key값을 저장한다.
	_pState->SetName(_strName);
}

CState* CFSM::FindState(const wstring& _strName)
{
	map<wstring, CState*>::iterator iter = m_mapState.find(_strName);
	if (iter != m_mapState.end())
		return iter->second;

	return nullptr;
}

void CFSM::ChangeState(const wstring& _strStateName)
{
	CState* pNextState = FindState(_strStateName);
	assert(pNextState);		// 해당 State가 없을 시 Assert

	// 현재 상태의 Exit 호출
	if (m_pCurState != nullptr)
		m_pCurState->Exit();

	// 현재 상태를 바꿀 상태로 치환
	m_pCurState = pNextState;

	// 다음 상태 Enter 호출
	m_pCurState->Enter();
}
