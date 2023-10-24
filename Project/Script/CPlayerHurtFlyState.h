#pragma once
#include <Engine\CState.h>

class CPlayerHurtFlyState :
    public CState
{
public:
    CPlayerHurtFlyState();
    ~CPlayerHurtFlyState();

public:
    CLONE(CPlayerHurtFlyState)

private:

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};

