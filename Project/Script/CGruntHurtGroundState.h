#pragma once
#include <Engine\CState.h>

class CGruntHurtGroundState :
    public CState
{
public:
    CGruntHurtGroundState();
    ~CGruntHurtGroundState();

public:
    CLONE(CGruntHurtGroundState)

private:

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;

};

