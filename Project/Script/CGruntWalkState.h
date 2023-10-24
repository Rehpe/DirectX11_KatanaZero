#pragma once
#include <Engine\CState.h>

class CGruntWalkState :
    public CState
{
public:
    CGruntWalkState();
    ~CGruntWalkState();

public:
    CLONE(CGruntWalkState)

private:

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;

};

