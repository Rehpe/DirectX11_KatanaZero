#pragma once
#include "CComponent.h"
class CCollider2D :
    public CComponent
{
public:
    CCollider2D();
    ~CCollider2D();

public:
    CLONE(CCollider2D);

private:
    bool            m_bColliderActivate;

    // AABB(Box-Collision)
    Vec3            m_vOffsetPos;       // 원본 오브젝트로부터 떨어진 거리
    Vec3            m_vOffsetScale;     // 원본 오브젝트의 크기에 비례한 배율 정보
    bool            m_bAbsolute;        // 원본 오브젝트의 크기에 비례하지 않는 절대값 설정

    // Line 충돌체의 경우
    Vec3            m_vStartPos;
    Vec3            m_vEndPos;

    COLLIDER2D_TYPE m_Shape;            // 충돌체 모양(사각형, 원, 선)

    Matrix          m_matCollider2D;    // 충돌체의 월드행렬

    int             m_iCollisionCount;  // 충돌 카운트(몇 개의 충돌체와 충돌중인지)

public:
    virtual void finaltick() override;

public:
    void SetColliderActivate(bool _b) { m_bColliderActivate = _b; }
    bool IsColliderActivate() { return m_bColliderActivate; }

    void SetOffsetPos(Vec2 _vOffsetPos) { m_vOffsetPos = Vec3(_vOffsetPos.x, _vOffsetPos.y, 0.f); }
    Vec3 GetOffsetPos() { return m_vOffsetPos; }
    
    void SetOffsetScale(Vec2 _vOffsetScale) { m_vOffsetScale = Vec3(_vOffsetScale.x, _vOffsetScale.y, 1.f); }
    Vec3 GetOffsetScale() { return m_vOffsetScale; }

    void SetStartPos(Vec3 _pos) { m_vStartPos = _pos; }
    Vec3 GetStartPos() { return m_vStartPos; }

    void SetEndPos(Vec3 _pos) { m_vEndPos = _pos; }
    Vec3 GetEndPos() { return m_vEndPos; }

    void SetAbsolute(bool _bSet) { m_bAbsolute = _bSet; }
    bool IsAbsolute() { return m_bAbsolute; }

    void SetCollider2DType(COLLIDER2D_TYPE _Type) { m_Shape = _Type; }
    COLLIDER2D_TYPE GetCollider2DType() { return m_Shape; }

    const Matrix& GetColliderWorldMat() { return m_matCollider2D; }

    Vec3 GetColliderWorldPos();

public:
    void BeginOverlap(CCollider2D* _Other);
    void OnOverlap(CCollider2D* _Other);
    void EndOverlap(CCollider2D* _Other);
   
public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
    
};

