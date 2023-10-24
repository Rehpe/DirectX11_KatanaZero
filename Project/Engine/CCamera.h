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

    float       m_fScale;           // ī�޶� ���� (���� ���������� ���)
    float       m_fAspectRatio;     // ��Ⱦ��(���� ��� ���� ����)
    
    Matrix      m_matView;          // �� ���
    Matrix      m_matProj;          // ���� ���

    int         m_iCamIdx;          // �����Ŵ����� ��ϵ� �ڽ��� Cam ��ȣ(ī�޶� �켱����)

    UINT        m_iLayerMask;       // ��Ʈ����ŷ���� �������� ������ �ʴ� ���̾�� ����ũó��(���̾� ������ 32���̹Ƿ� int)


    // ������ �������� ������Ʈ �з��� ����
    vector<CGameObject*>    m_vecOpaque;
    vector<CGameObject*>    m_vecMask;
    vector<CGameObject*>    m_vecTransparent;
    vector<CGameObject*>    m_vecPost;
    vector<CGameObject*>    m_vecUI;

public:
    // 64����Ʈ�� �뷮�� ũ�� ������ const ���۷����� �޾ƿ´�
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

