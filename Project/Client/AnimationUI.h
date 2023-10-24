#pragma once
#include "UI.h"


class AnimationUI :
    public UI
{
public:
    AnimationUI();
    ~AnimationUI();

private:
    ComPtr<ID3D11ShaderResourceView>    m_AtlasSRV;
    CGameObject*                        m_pTargetObj;

    int                                 m_iCol;
    int                                 m_iRow;

    bool                                m_bPause;
    bool                                m_bRepeat;

    ImVec2          m_vStartPos;
    ImVec2          m_vEndPos;



public:
    virtual void finaltick() override;
    virtual int render_update() override;

    void Reset(const string& _strName, ImVec2 _vSize);

    bool DragRect(int _MouseBtn);

public:
    void SetTarget(CGameObject* _Target) { m_pTargetObj = _Target; }
};

