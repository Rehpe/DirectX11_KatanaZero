#pragma once
#include <Engine\CState.h>

class CPlayerPostCrouchState :
    public CState
{
public:
    CPlayerPostCrouchState();
    ~CPlayerPostCrouchState();

public:
    CLONE(CPlayerPostCrouchState)

private:

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};

