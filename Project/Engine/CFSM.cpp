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
	// FSM�� ������ ���� State�� �����ϰ�, ���ุ �����ش�
	if (m_pCurState == nullptr)
		return;

	m_pCurState->finaltick();
}

void CFSM::AddState(const wstring& _strName, CState* _pState)
{
	// State �ʿ� �ش� State�� �߰��Ѵ�
	m_mapState.insert(make_pair(_strName, _pState));

	// �ش� state�� ���� FSM�� �������� �����Ѵ�.
	_pState->m_pOwnerFSM = this;

	// �ش� State�� Key���� �����Ѵ�.
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
	assert(pNextState);		// �ش� State�� ���� �� Assert

	// ���� ������ Exit ȣ��
	if (m_pCurState != nullptr)
		m_pCurState->Exit();

	// ���� ���¸� �ٲ� ���·� ġȯ
	m_pCurState = pNextState;

	// ���� ���� Enter ȣ��
	m_pCurState->Enter();
}
