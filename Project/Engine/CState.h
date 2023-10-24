#pragma once
#include "CEntity.h"

class CFSM;
class CGameObject;

class CState :
    public CEntity
{
public:
    CState();
    virtual ~CState();

    friend class CFSM;

public:
    CLONE_DEACTIVATE(CState);

private:
    CFSM*       m_pOwnerFSM;

public:
    //State�� ���� ���·� ��ȯ���� �� �� ��(finaltick), ���԰� ���� �Լ��� �ʼ��� �������Ѵ�
    virtual void finaltick();
    virtual void Enter();
    virtual void Exit();

public:
    CFSM*        GetOwnerFSM()  { return m_pOwnerFSM; }
    CGameObject* GetOwner();

};

