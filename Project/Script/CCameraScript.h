#pragma once
#include <Engine\CScript.h>

class CCameraScript :
    public CScript
{
public:
    CCameraScript();
    ~CCameraScript();

public:
    CLONE(CCameraScript);

private:
    Vec3    m_vPlayerPos;
    Vec3    m_vCurrentPos;
    Vec3    m_vCameraOffset;
    float   m_fSmoothFactor;

    Vec2    m_vMinPos;
    Vec2    m_vMaxPos;

    // Camera Shake
    bool    m_bShakeCamera;
    float   m_fShakeDuration;
    float   m_fShakeMagnitude;
    float   m_fShakeTimer;


public:
    virtual void begin() override;
    virtual void tick() override;

public:
    void InitFramePos();
    void ApplyCameraShake(float Duration, float Magnitude);

};

