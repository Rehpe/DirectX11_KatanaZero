#pragma once
#include <Engine\CState.h>

class CPlayerRunState :
    public CState
{
public:
    CPlayerRunState();
    ~CPlayerRunState();

public:
    CLONE(CPlayerRunState)

private:

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};

