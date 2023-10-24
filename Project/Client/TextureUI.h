#pragma once
#include "ResourceUI.h"
class TextureUI :
    public ResourceUI
{
public:
    TextureUI();
    ~TextureUI();

public:
    virtual int render_update() override;
};

