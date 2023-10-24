#pragma once
#include <Engine\CScript.h>

class CCameraMoveScript :
    public CScript
{
public:
    CCameraMoveScript();
    ~CCameraMoveScript();

public:
    CLONE(CCameraMoveScript);

private:
    float       m_fCamSpeed;

private:
    void Camera2DMove();
    void Camera3DMove();

public:
    virtual void tick() override;

};

