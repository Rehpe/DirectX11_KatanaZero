#pragma once
#include <Engine\CState.h>

class CGangsterAttackState :
    public CState
{
public:
    CGangsterAttackState();
    ~CGangsterAttackState();

public:
    CLONE(CGangsterAttackState)

private:

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};

