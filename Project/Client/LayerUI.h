#pragma once
#include "UI.h"

#include <Engine\CLayer.h>

class LayerUI :
    public UI
{
public:
    LayerUI();
    ~LayerUI();

private:
    CGameObject*        m_pTargetObj;

    vector<string>     m_LayerNameVec;

    const char*         m_LayerNameArr[MAX_LAYER];
    int                 m_curLayerIdx;

public:
    virtual int render_update() override;

public:
    void SetTargetObj(CGameObject* _Obj) { m_pTargetObj = _Obj; }


};

