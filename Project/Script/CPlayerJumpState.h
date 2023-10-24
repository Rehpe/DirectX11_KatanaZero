#pragma once
#include <Engine\CState.h>

class CPlayerJumpState :
    public CState
{
public:
    CPlayerJumpState();
    ~CPlayerJumpState();

public:
    CLONE(CPlayerJumpState)

private:

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;

};

