#include "pch.h"
#include "GraphicsShaderUI.h"

GraphicsShaderUI::GraphicsShaderUI()
    : ResourceUI(RES_TYPE::GRAPHICS_SHADER)
{
    SetName("GraphicsShader");
}
GraphicsShaderUI::~GraphicsShaderUI()
{
}

int GraphicsShaderUI::render_update()
{
    ResourceUI::render_update();
    return 0;
}
