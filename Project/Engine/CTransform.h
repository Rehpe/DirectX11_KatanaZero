#pragma once
#include "CComponent.h"
class CTransform :
    public CComponent
{
public:
    CTransform();
    ~CTransform();

public:
    CLONE(CTransform);

private:
    Vec3    m_vRelativePos;
    Vec3    m_vRelativeScale;
    Vec3    m_vRelativeRot;

    Vec3    m_vRelativeDir[3];  // ���� ����(������(Right_X��� ����), ��(Up_Y��� ����), ����(Front_Z��� ����) ����)
    Vec3    m_vWorldDir[3];     // ���� ���� ����

    Matrix  m_matWorldScale;    // ������Ʈ�� ���� ��Ŀ��� ũ�� ��ĸ� ��� ��
    Matrix  m_matWorld;         // ������Ʈ�� ���� ��� (ũ��, ȸ��, �̵� ������ ���ĳ���)

    bool    m_bAbsolute;        // �θ� ũ�⿡ ������ ���� �ʴ� �������� �̵�, ũ�Ⱚ

public:
    virtual void finaltick() override;
    void UpdateData();

public:
    void SetRelativePos(Vec3 _vPos) { m_vRelativePos = _vPos; }
    void SetRelativeScale(Vec3 _vScale) { m_vRelativeScale = _vScale; }
    void SetRelativeRot(Vec3 _vRot) { m_vRelativeRot = _vRot; }

    void SetRelativePos(float _x, float _y, float _z) { m_vRelativePos = Vec3(_x, _y, _x); }
    void SetRelativeScale(float _x, float _y, float _z) { m_vRelativeScale = Vec3(_x, _y, _x); }
    void SetRelativeRot(float _x, float _y, float _z) { m_vRelativeRot = Vec3(_x, _y, _x); }

    Vec3 GetRelativePos() const { return m_vRelativePos; };
    Vec3 GetRelativeScale() const { return m_vRelativeScale; };
    Vec3 GetRelativeRot() const { return m_vRelativeRot; };
    
    Vec3 GetWorldPos() const { return m_matWorld.Translation(); }   // �̵����� �����Ͽ� �������� 4�࿡ ���������Ƿ�,
                                                                    // ���� ������Ŀ��� 41, 42, 43 �κ��� �ٷ� ����������

    Vec3 GetRelativeDir(DIR_TYPE _type) const { return m_vRelativeDir[(UINT)_type]; }
    Vec3 GetWorldDir(DIR_TYPE _type) const { { return m_vWorldDir[(UINT)_type]; } }

    const Matrix& GetWorldScaleMat() { return m_matWorldScale;  }
    const Matrix& GetWorldMat() { return m_matWorld; }

    void SetWorldMat(const Matrix& _mat) { m_matWorld = _mat; }

    void SetAbsolute(bool _b) { m_bAbsolute = _b; }
    bool IsAbsolute() { return m_bAbsolute; }

public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
};

