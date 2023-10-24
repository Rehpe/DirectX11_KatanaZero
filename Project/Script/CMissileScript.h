#pragma once
#include <Engine\CScript.h>

class CMissileScript :
    public CScript
{
public:
    CMissileScript();
    ~CMissileScript();

public:
    CLONE(CMissileScript);

private:
    Vec3        m_vDir;         // 방향
    float       m_fSpeed;       // 속력

public:
    virtual void tick() override;

public:
    void SetSpeed(float _fSpeed) { m_fSpeed = _fSpeed; }




};

