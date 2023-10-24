#pragma once
#include <Engine\CState.h>

class CPlayerWallSlideState :
    public CState
{
public:
    CPlayerWallSlideState();
    ~CPlayerWallSlideState();

public:
    CLONE(CPlayerWallSlideState)

private:
    float   m_fAntiGravityTime;

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};

