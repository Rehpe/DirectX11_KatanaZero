#pragma once
#include <Engine\CState.h>

class CPlayerHurtGroundState :
    public CState
{
public:
    CPlayerHurtGroundState();
    ~CPlayerHurtGroundState();

public:
    CLONE(CPlayerHurtGroundState)

private:

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};

