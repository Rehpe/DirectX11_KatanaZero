#pragma once
#include "ComponentUI.h"

class CAnim2D;
class CTexture;

class Animator2DUI :
    public ComponentUI
{
public:
    Animator2DUI();
    ~Animator2DUI();

private:

    Ptr<CTexture>                           m_EditDSTex;
    vector<Ptr<CTexture>>                   m_vecAnimFrm;

    ComPtr<ID3D11ShaderResourceView>        m_AtlasSRV;
    Ptr<CTexture>                           m_EditTex;
    map<wstring, CAnim2D*>                  m_mapAnim;
    int                                     m_iSelectedAnimIdx;
    CAnim2D*                                m_pCurAnim;

    // ===== Editing Anim Info =====
    CAnim2D*                                m_pEditingAnim;

    //Vec2                                    m_vEditingLeftTopPx;
    //Vec2                                    m_vEditingSlicePx;
    //Vec2                                    m_vEditingOffsetPx;
    //Vec2                                    m_vEditingBackSizePx;
    //int                                     m_iEditingFPS;
    bool                                    m_bEditingPause;
    bool                                    m_bEditingRepeat;

    bool                                m_Changed;

    int                                 m_iCol;
    int                                 m_iRow;

    ImVec2                              m_vRectStartPos;
    ImVec2                              m_vRectEndPos;

    bool                                m_bPause;
    bool                                m_bRepeat;

    // Grid Option
    bool                                m_bShowGrid;
    int                                 m_iGridRow;
    int                                 m_iGridCol;

    // Overlay Option
    bool                                m_bShowOverlay;
    float                               m_fTransparency;

public:
    virtual void init() override;
    virtual int render_update() override;

public:
    void PlayAnim();
    
    void CreateNewAnim(DWORD_PTR _AtlasKey);

    void CreateFrameImage(CAnim2D* _curAnim);

    bool DragRect(int _MouseBtn);

    void DrawGrid(ImVec2 imagePos, ImVec2 imageSize, int _iRow, int _iCol);

    void DrawOverlay(ImVec2 _imageSize, float _transparency);
};

