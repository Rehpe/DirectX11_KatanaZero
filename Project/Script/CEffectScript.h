#pragma once
#include <Engine\CScript.h>

class CEffectScript :
    public CScript
{
public:
    CEffectScript();
    ~CEffectScript();

public:
    CLONE(CEffectScript);

private:
    UINT       m_EffectType;
    Vec3       m_vPos;
    Vec2       m_vDir;
    float      m_fTimer;

public:
    virtual void begin() override;
    virtual void tick() override;

public:
    void    SetEffectType(UINT _Type) { m_EffectType = _Type; };
    void    SetPos(Vec3 _vPos) { m_vPos = _vPos; }
    void    SetDir(Vec2 _vDir) { m_vDir = _vDir; }


};

