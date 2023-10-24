#pragma once
#include <Engine\CState.h>

class CPlayerRuntoIdleState :
    public CState
{
public:
    CPlayerRuntoIdleState();
    ~CPlayerRuntoIdleState();

public:
    CLONE(CPlayerRuntoIdleState)

private:

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};

