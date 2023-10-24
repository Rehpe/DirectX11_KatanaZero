#pragma once
#include <Engine\CState.h>

class CPlayerIdleState :
    public CState
{
public:
    CPlayerIdleState();
    ~CPlayerIdleState();

public:
    CLONE(CPlayerIdleState)

private:

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};

