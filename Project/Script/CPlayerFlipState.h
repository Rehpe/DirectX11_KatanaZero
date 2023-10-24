#pragma once
#include <Engine\CState.h>

class CPlayerFlipState :
    public CState
{
public:
    CPlayerFlipState();
    ~CPlayerFlipState();

public:
    CLONE(CPlayerFlipState)

private:
    UINT        m_iPreviousWall;

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;
};

