#pragma once
#include <Engine\CState.h>

class CGangsterIdleState :
    public CState
{
public:
    CGangsterIdleState();
    ~CGangsterIdleState();

public:
    CLONE(CGangsterIdleState)

private:

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};

