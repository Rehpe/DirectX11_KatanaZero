#pragma once
#include <Engine\CState.h>

class CPlayerIdletoRunState :
    public CState
{
public:
    CPlayerIdletoRunState();
    ~CPlayerIdletoRunState();

public:
    CLONE(CPlayerIdletoRunState)

private:

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;

};

