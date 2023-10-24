#pragma once
#include "CComponent.h"

class CRigidBody2D :
    public CComponent
{
public:
    CRigidBody2D();
    CRigidBody2D(const CRigidBody2D& _Other);
    ~CRigidBody2D();

public:
    CLONE(CRigidBody2D);

private:
    float               m_fMass;            // 질량
    Vec3                m_vVelocity;        // 속도
    Vec3                m_vForce;           // 가해지는 힘
    Vec3                m_vMaxVelocity;     // 제한 속도
    float               m_fFriction;        // 마찰력
    float               m_fSlopeAngle;      // 지면의 기울기

    Vec3                m_vecRampNormal;    // 경사로에 있을 때, 경사로의 수직인 법선

    // ===== 중력 관련 변수(플랫포머)
    bool                m_bIsGravity;       // 중력의 영향을 받는가?
    bool                m_bGround;          // 오브젝트가 땅 위에 있는가?
    bool                m_bOnSlope;         // 오브젝트가 경사로 위에 있는가?

    float               m_fGravityScale;

    bool                m_bMovingInput;    // 오브젝트가 움직임 Input을 받고 있는가?
    float               m_fGravityAccel;    // 중력 가속도
    float               m_fGravityVLimit;   // 중력에 의한 제한 속도

public:
    virtual void finaltick() override;

public:
    void SetGround(bool _b);
    void SetOnSlope(bool _b);
    void SetMovingInput(bool _b) { m_bMovingInput = _b; }
   
    void SetRampNormal(Vec3 _vecNormal) { m_vecRampNormal = _vecNormal; }
    Vec3 GetRampNormal() { return m_vecRampNormal; }

public:
    float GetMass() { return m_fMass; }
    void  SetMass(float _f) { m_fMass = _f; }

public:
    void AddForce(Vec3 _vForce) { m_vForce += _vForce; }
    Vec3 GetForce() { return m_vForce; }

    Vec3 GetVelocity() { return m_vVelocity; }
    void SetVelocity(Vec3 _vVelocity) { m_vVelocity = _vVelocity; }
    void SetVelocityX(float _f) { m_vVelocity.x = _f; }
    void SetVelocityY(float _f) { m_vVelocity.y = _f; }

    void AddVelocity(Vec3 _vAddV) { m_vVelocity += _vAddV; }

    void SetGravityAccel(float _fAccel) { m_fGravityAccel = _fAccel; }
    float GetGravityAccel() { return m_fGravityAccel; }

    void SetGravity(bool _b) { m_bIsGravity = _b; }
    bool IsGravity() { return m_bIsGravity; }

    void SetGravityScale(float _f) { m_fGravityScale = _f; }

    void SetSlopeAngle(float _fAngle) { m_fSlopeAngle = _fAngle; }
    float GetSlopeAngle() { return m_fSlopeAngle; }

    void SetVelocityLimit(Vec3 _vMaxVelocity) { m_vMaxVelocity = _vMaxVelocity; }
    void SetVelocityLimitX(float _x) { m_vMaxVelocity.x = _x; }
    void SetVelocityLimitY(float _y) { m_vMaxVelocity.y = _y; }


    void SetGravityVelocityLimit(float _fLimit) { m_fGravityVLimit = _fLimit; }

    bool IsGround() { return m_bGround; };
    bool IsOnSlope() { return m_bOnSlope; }
    bool IsMovingInput() { return m_bMovingInput; }

    void ResetGravityAccel() { m_fGravityAccel = 900.f; }

public:
    void CalcFriction();


public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;


};

