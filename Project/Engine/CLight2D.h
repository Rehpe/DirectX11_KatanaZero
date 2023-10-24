#pragma once
#include "CComponent.h"

// 광원 컴포넌트. 게임 전체에 영향을 주는 전역광원 / 근처만 밝히는 일반광원 으로 나뉜다.

class CLight2D :
    public CComponent
{
public:
    CLight2D();
    ~CLight2D();

public:
    CLONE(CLight2D);

private:
    tLightInfo      m_LightInfo;

public:
    virtual void finaltick() override;

public:
    void SetLightType(LIGHT_TYPE _Type) { m_LightInfo.LightType = (UINT)_Type; }
    void SetLightDirection(Vec3 _vDir) { m_LightInfo.vWorldDir = _vDir; }
    void SetLightDiffuse(Vec3 _vDiffuse) { m_LightInfo.Color.vDiffuse = _vDiffuse; }
    void SetLightAmbient(Vec3 _vAmbient) { m_LightInfo.Color.vAmbient = _vAmbient; }
    void SetIntensity(float _intensity) { m_LightInfo.Intensity = _intensity; }
    void SetRadius(float _Radius) { m_LightInfo.Radius = _Radius; }
    void SetAngle(float _Angle) { m_LightInfo.Angle = _Angle; }

    UINT GetLightType() { return m_LightInfo.LightType; }
    Vec3 GetLightDirection() { return m_LightInfo.vWorldDir; }
    Vec3 GetLightDiffuse() { return m_LightInfo.Color.vDiffuse; }
    Vec3 GetLightAmbient() { return m_LightInfo.Color.vAmbient; }
    float GetRadius() { return m_LightInfo.Radius; }
    float GetIntensity() { return m_LightInfo.Intensity; }
    float GetAngle() { return m_LightInfo.Angle; }

public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
};

