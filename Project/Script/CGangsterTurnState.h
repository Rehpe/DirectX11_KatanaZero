#pragma once
#include <Engine\CState.h>

class CGangsterTurnState :
    public CState
{
public:
    CGangsterTurnState();
    ~CGangsterTurnState();

public:
    CLONE(CGangsterTurnState)

private:

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};

