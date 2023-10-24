#pragma once
#include <Engine\CState.h>

class CPlayerCrouchState :
    public CState
{
public:
    CPlayerCrouchState();
    ~CPlayerCrouchState();

public:
    CLONE(CPlayerCrouchState)

private:

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};

