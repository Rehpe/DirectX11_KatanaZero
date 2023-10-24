#pragma once
#include <Engine\CState.h>

class CPlayerFallState :
    public CState
{
public:
    CPlayerFallState();
    ~CPlayerFallState();

public:
    CLONE(CPlayerFallState)

private:

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};

