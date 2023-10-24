#pragma once
#include "ResourceUI.h"
class MaterialUI :
    public ResourceUI
{
public:
    MaterialUI();
    ~MaterialUI();

private:
    TEX_PARAM       m_eSelected;        // �� �� �ؽ�ó�� ���ε��ߴ���

public:
    virtual int render_update() override;

public:
    void SelectTexture(DWORD_PTR _Key);
};

