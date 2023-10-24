#pragma once
#include <Engine\CState.h>

class CPlayerRollState :
    public CState
{
public:
    CPlayerRollState();
    ~CPlayerRollState();

public:
    CLONE(CPlayerRollState)

private:

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};

