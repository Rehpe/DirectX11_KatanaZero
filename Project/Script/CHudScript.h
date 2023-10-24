#pragma once
#include <Engine\CScript.h>

class CGameManagerScript;

class CHudScript :
    public CScript
{
public:
    CHudScript();
    ~CHudScript();

public:
    CLONE(CHudScript);

private:
    bool                            m_bInit;
    CGameManagerScript*             m_GMScript;
    vector<CGameObject*>            m_arrBattery;
    CGameObject*                    m_Timer;
 

public:
    virtual void begin() override;
    virtual void tick() override;

    void init();
    

};

