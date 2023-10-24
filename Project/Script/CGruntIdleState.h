#pragma once
#include <Engine\CState.h>

class CGruntIdleState :
    public CState
{
public:
    CGruntIdleState();
    ~CGruntIdleState();

public:
    CLONE(CGruntIdleState)

private:

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};

