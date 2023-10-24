#pragma once
#include <Engine\CScript.h>

class CLaserSwitchScript :
    public CScript
{
public:
    CLaserSwitchScript();
    ~CLaserSwitchScript();

public:
    CLONE(CLaserSwitchScript);

private:
    vector<CGameObject*>    m_Lasers;
    bool                    m_bActive;

public:
    virtual void begin() override;
    virtual void tick() override;

public:
    virtual void OnOverlap(CCollider2D* _Other);
};

