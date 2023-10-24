#pragma once
#include "ResourceUI.h"
class GraphicsShaderUI :
    public ResourceUI
{
public:
    GraphicsShaderUI();
    ~GraphicsShaderUI();

public:
    virtual int render_update() override;
};

