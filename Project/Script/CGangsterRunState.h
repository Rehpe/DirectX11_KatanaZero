#pragma once
#include <Engine\CState.h>

class CGangsterRunState :
    public CState
{
public:
    CGangsterRunState();
    ~CGangsterRunState();

public:
    CLONE(CGangsterRunState)

private:

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;

public:
    void MoveAlongRamp(const Vec3& _StartPos, const Vec3& _EndPos);
};

