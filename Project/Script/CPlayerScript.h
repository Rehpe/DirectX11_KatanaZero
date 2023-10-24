#pragma once
#include <Engine\CScript.h>

class CPlayerScript :
    public CScript
{
public:
    CPlayerScript();
    ~CPlayerScript();

public:
    CLONE(CPlayerScript);

private:
    int         m_iHp;
    float       m_fSpeed;
    float       m_fJumpForce;
    float       m_fFallingGravity;
    float       m_fOnAirTime;
    float       m_fAttackTimer;
    float       m_fAttackCoolDown;

    // Input Check
    int         m_iLookDir;     // 캐릭터 방향( 1: 오른쪽, -1: 왼쪽)
    int         m_iHorizontalInput;
    int         m_iVerticalInput;
    bool        m_bInput;   // 인풋 여부

    bool        m_bDead;
    bool        m_bGround;
    bool        m_bOnSlope;
    bool        m_bIsJump;
    bool        m_bIsFall;
    bool        m_bIsRoll;
    bool        m_bIsWallGrab;
    bool        m_bIsFlip;
    bool        m_bIsAttack;
    bool        m_bIsAttackable;

    bool        m_bPlatformBreakDown;

    Vec3        m_vKnockBackDir;
    float       m_fKnockBackForce;
    Vec2        m_vAttackDir;

    float m_fForceDuration;
    float m_fForceCounter;
    Vec3 m_vForce;
    bool m_bApplyForce;

    // 지형 정보
    tPlatformColliderInfo   m_PlatformInfo;

    // 충돌 중인 벽 정보
    int        m_arrWallCollision[(UINT)WALL_TYPE::END];

public:
    virtual void begin() override;
    virtual void tick() override;

public:
    virtual void BeginOverlap(CCollider2D* _Other);
    virtual void OnOverlap(CCollider2D* _Other);
    virtual void EndOverlap(CCollider2D* _Other);

public:
    void CalcKnockBackDir(CCollider2D* _Other);

private:
    void CheckState();
    void CheckInput();
    void CheckOnAirTime();
    void PlayerMove();
    void CheckWall();
    Vec3 GetPositionOnRamp();
    Vec2 CalcClickDir();

    void ResetState();
  

public:
    bool IsDead() { return m_bDead; }
    bool IsOnGround() { return m_bGround; }
    bool IsOnSlope() { return m_bOnSlope; }
    bool IsJump() { return m_bIsJump; }
    bool IsFall() { return m_bIsFall; }
    bool IsFlip() { return m_bIsFlip; }
    bool IsRoll() { return m_bIsRoll; }
    int GetInputX() { return m_iHorizontalInput; }
    int GetInputY() { return m_iVerticalInput; }
    bool IsInput() { return m_bInput; }
    bool IsAttackable() { return m_bIsAttackable; }
    bool IsPlatformBreakdown() { return m_bPlatformBreakDown; }

    void ReduceHp(int _i) { m_iHp -= _i; }
    int  GetHp() { return m_iHp; }

    Vec3 GetKnockBackDir() { return m_vKnockBackDir; }
    float GetKnockBackForce() { return m_fKnockBackForce; }

    void SetDead(bool _b) { m_bDead = _b; }
    void SetJump(bool _b) { m_bIsJump = _b; }
    void SetFall(bool _b) { m_bIsFall = _b; }
    void SetRoll(bool _b) { m_bIsRoll = _b; }
    void SetWallGrab (bool _b) { m_bIsWallGrab = _b; }
    void SetFlip(bool _b) { m_bIsFlip = _b; }
    void SetAttack(bool _b) { m_bIsAttack = _b; }
    void SetAttackable(bool _b) { m_bIsAttackable = _b; }
    
    void SetPlatformInfo(tPlatformColliderInfo _info) { m_PlatformInfo = _info; }
    tPlatformColliderInfo GetPlatformInfo() { return m_PlatformInfo; }

    const int*  GetWallCollision() const { return m_arrWallCollision; }
    void SetWallCollision(UINT _wallType, int _bool) { m_arrWallCollision[(UINT)_wallType] = _bool; }
    void ResetWallCollision();

    void SetLookDir(int _i) { m_iLookDir = _i; }
    int GetLookDir() { return m_iLookDir; }

    void SetAttackDir(Vec2 _Dir) { m_vAttackDir = _Dir; }
    Vec2 GetAttackDir() { return m_vAttackDir; }

public:
    float GetSpeed() { return m_fSpeed; }
    void SetSpeed(float _f) { m_fSpeed = _f; }

    Vec2  GetMouseDir() { return CalcClickDir(); }

public:
    void CreateShadow();
    void CreateWhiteShadow();
    
public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
};

