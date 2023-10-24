#pragma once
#include "ResourceUI.h"
class MaterialUI :
    public ResourceUI
{
public:
    MaterialUI();
    ~MaterialUI();

private:
    TEX_PARAM       m_eSelected;        // 몇 번 텍스처에 바인딩했는지

public:
    virtual int render_update() override;

public:
    void SelectTexture(DWORD_PTR _Key);
};

