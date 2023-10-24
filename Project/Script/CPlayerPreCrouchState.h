#pragma once
#include <Engine\CState.h>

class CPlayerPreCrouchState :
    public CState
{
public:
    CPlayerPreCrouchState();
    ~CPlayerPreCrouchState();

public:
    CLONE(CPlayerPreCrouchState)

private:

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};

