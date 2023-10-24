#include "pch.h"
#include "ComputeShaderUI.h"

ComputeShaderUI::ComputeShaderUI()
    : ResourceUI(RES_TYPE::COMPUTE_SHADER)
{
    SetName("ComputeShader");
}

ComputeShaderUI::~ComputeShaderUI()
{
}

int ComputeShaderUI::render_update()
{
    ResourceUI::render_update();
    return 0;
}
