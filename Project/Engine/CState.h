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
    //State는 본인 상태로 전환됐을 때 할 일(finaltick), 진입과 퇴장 함수를 필수로 가져야한다
    virtual void finaltick();
    virtual void Enter();
    virtual void Exit();

public:
    CFSM*        GetOwnerFSM()  { return m_pOwnerFSM; }
    CGameObject* GetOwner();

};

