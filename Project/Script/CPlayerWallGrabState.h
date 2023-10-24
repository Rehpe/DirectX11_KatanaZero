#pragma once
#include <Engine\CState.h>

class CPlayerWallGrabState :
    public CState
{
public:
    CPlayerWallGrabState();
    ~CPlayerWallGrabState();

public:
    CLONE(CPlayerWallGrabState)

private:
    float       m_fAntiGravityTime;

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};

