#pragma once
#include <Engine\CState.h>

class CGangsterHurtFlyState :
    public CState
{
public:
    CGangsterHurtFlyState();
    ~CGangsterHurtFlyState();

public:
    CLONE(CGangsterHurtFlyState)

private:

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};

