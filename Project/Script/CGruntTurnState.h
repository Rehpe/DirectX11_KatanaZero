#pragma once
#include <Engine/CState.h>

class CGruntTurnState :
    public CState
{
public:
    CGruntTurnState();
    ~CGruntTurnState();

public:
    CLONE(CGruntTurnState)

private:

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};

