#pragma once
#include "CComponent.h"
class CCamera :
    public CComponent
{
public:
    CCamera();
    CCamera(const CCamera& _Other);
    ~CCamera();

public:
    CLONE(CCamera);

private:
    PROJ_TYPE   m_ProjType;

    float       m_fScale;           // 카메라 배율 (직교 투영에서만 사용)
    float       m_fAspectRatio;     // 종횡비(세로 대비 가로 비율)
    
    Matrix      m_matView;          // 뷰 행렬
    Matrix      m_matProj;          // 투영 행렬

    int         m_iCamIdx;          // 렌더매니저에 등록된 자신의 Cam 번호(카메라 우선순위)

    UINT        m_iLayerMask;       // 비트마스킹으로 렌더링을 원하지 않는 레이어는 마스크처리(레이어 갯수가 32개이므로 int)


    // 도메인 설정별로 오브젝트 분류할 벡터
    vector<CGameObject*>    m_vecOpaque;
    vector<CGameObject*>    m_vecMask;
    vector<CGameObject*>    m_vecTransparent;
    vector<CGameObject*>    m_vecPost;
    vector<CGameObject*>    m_vecUI;

public:
    // 64바이트로 용량이 크기 때문에 const 레퍼런스로 받아온다
    const Matrix& GetViewMat() { return m_matView; }
    const Matrix& GetProjMat() { return m_matProj; }

    UINT GetLayerMask() { return m_iLayerMask; }
    //void SetLayerMask(UINT _mask) { m_iLayerMask = _mask; }

    void SetProjType(PROJ_TYPE _type) { m_ProjType = _type; }
    PROJ_TYPE GetProjType() { return m_ProjType; }

    void SetCameraIndex(int _idx);
    int  GetCameraIndex() { return m_iCamIdx; }

    void SetLayerMask(int _Layer, bool _Visible);
    void SetLayerMaskAll(bool _Visible);

    void SetScale(float _fScale) { m_fScale = _fScale; }
    float GetScale() { return m_fScale; }


public:
    void SortObject();
    void render();

public:
    virtual void begin() override;
    virtual void finaltick() override;

private:
    void CalcViewMat();
    void CalcProjMat();

    void clear();
    void render_opaque();
    void render_mask();
    void render_transparent();
    void render_postprocess();
    void render_ui();

public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;


};

