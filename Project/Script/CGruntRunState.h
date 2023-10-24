#pragma once
#include <Engine\CState.h>

class CGruntRunState :
    public CState
{
public:
    CGruntRunState();
    ~CGruntRunState();

public:
    CLONE(CGruntRunState)

private:
  

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;

private:
    void MoveAlongRamp(const Vec3& _StartPos, const Vec3& _EndPos);
};

