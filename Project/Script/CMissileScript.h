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
    Vec3        m_vDir;         // ����
    float       m_fSpeed;       // �ӷ�

public:
    virtual void tick() override;

public:
    void SetSpeed(float _fSpeed) { m_fSpeed = _fSpeed; }




};

