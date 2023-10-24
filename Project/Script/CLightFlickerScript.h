#pragma once
#include <Engine\CScript.h>

class CLightFlickerScript :
    public CScript
{
public:
    CLightFlickerScript();
    ~CLightFlickerScript();

public:
    CLONE(CLightFlickerScript);

private:
    float       m_fMaxIntensity;
    Vec3        m_fOriginalDiffuse;
    Vec3        m_fCurDiffuse;
    float       m_fSpeed;
    float       m_fTimer;

public:
    virtual void begin() override;
    virtual void tick() override;

public:
    void SetCurDiffuse(Vec3 _Col) { m_fCurDiffuse = _Col; }
    void ResetDiffuse() { m_fCurDiffuse = m_fOriginalDiffuse; }
};

