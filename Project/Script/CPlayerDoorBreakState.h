#pragma once
#include <Engine\CState.h>

class CPlayerDoorBreakState :
    public CState
{
public:
    CPlayerDoorBreakState();
    ~CPlayerDoorBreakState();

public:
    CLONE(CPlayerDoorBreakState)

private:

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};

