#pragma once
#include <Engine\CState.h>

class CGangsterWalkState :
    public CState
{
public:
    CGangsterWalkState();
    ~CGangsterWalkState();

public:
    CLONE(CGangsterWalkState)

private:

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};

