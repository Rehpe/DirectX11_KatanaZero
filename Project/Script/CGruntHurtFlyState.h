#pragma once
#include <Engine\CState.h>

class CGruntHurtFlyState :
    public CState
{
public:
    CGruntHurtFlyState();
    ~CGruntHurtFlyState();

public:
    CLONE(CGruntHurtFlyState)

private:

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;

};

