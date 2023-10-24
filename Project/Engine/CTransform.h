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

    Vec3    m_vRelativeDir[3];  // 방향 벡터(오른쪽(Right_X축과 평행), 위(Up_Y축과 평행), 전방(Front_Z축과 평행) 방향)
    Vec3    m_vWorldDir[3];     // 월드 방향 벡터

    Matrix  m_matWorldScale;    // 오브젝트의 월드 행렬에서 크기 행렬만 떼어낸 것
    Matrix  m_matWorld;         // 오브젝트의 월드 행렬 (크기, 회전, 이동 정보를 합쳐놓음)

    bool    m_bAbsolute;        // 부모 크기에 영향을 받지 않는 절대적인 이동, 크기값

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
    
    Vec3 GetWorldPos() const { return m_matWorld.Translation(); }   // 이동값은 누적하여 곱해져도 4행에 남아있으므로,
                                                                    // 최종 월드행렬에서 41, 42, 43 부분이 바로 월드포지션

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

