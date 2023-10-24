#pragma once
#include "ResourceUI.h"
class ComputeShaderUI :
    public ResourceUI
{
public:
    ComputeShaderUI();
    ~ComputeShaderUI();

public:
    virtual int render_update() override;
};

