#pragma once
#include <Engine\CState.h>

class CGangsterAimState :
    public CState
{
public:
    CGangsterAimState();
    ~CGangsterAimState();

public:
    CLONE(CGangsterAimState)

private:

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};

