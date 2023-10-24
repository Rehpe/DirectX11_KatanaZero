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
    Vec3            m_vOffsetPos;       // ���� ������Ʈ�κ��� ������ �Ÿ�
    Vec3            m_vOffsetScale;     // ���� ������Ʈ�� ũ�⿡ ����� ���� ����
    bool            m_bAbsolute;        // ���� ������Ʈ�� ũ�⿡ ������� �ʴ� ���밪 ����

    // Line �浹ü�� ���
    Vec3            m_vStartPos;
    Vec3            m_vEndPos;

    COLLIDER2D_TYPE m_Shape;            // �浹ü ���(�簢��, ��, ��)

    Matrix          m_matCollider2D;    // �浹ü�� �������

    int             m_iCollisionCount;  // �浹 ī��Ʈ(�� ���� �浹ü�� �浹������)

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

