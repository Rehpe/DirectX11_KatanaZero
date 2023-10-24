#pragma once
#include <Engine\CState.h>

class CGruntAttackState :
    public CState
{
public:
    CGruntAttackState();
    ~CGruntAttackState();

public:
    CLONE(CGruntAttackState);

private:

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;

};

