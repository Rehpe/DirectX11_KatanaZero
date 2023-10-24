#pragma once
#include <Engine\CState.h>

class CPlayerAttackState :
    public CState
{
public:
    CPlayerAttackState();
    ~CPlayerAttackState();

public:
    CLONE(CPlayerAttackState)

private:
    float       m_fAttackPower;
    Vec2        m_vecAttackDir;

public:
    virtual void finaltick() override;
    virtual void Enter() override;
    virtual void Exit() override;

public:
    void SetAttackDir(Vec2 _Dir) { m_vecAttackDir = _Dir; }

public:
    Vec2 CalcClickDir();
};

