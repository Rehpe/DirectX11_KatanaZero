#pragma once
#include <Engine\CState.h>

class CGangsterHurtGroundState :
    public CState
{
public:
    CGangsterHurtGroundState();
    ~CGangsterHurtGroundState();

public:
    CLONE(CGangsterHurtGroundState);

private:

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};

