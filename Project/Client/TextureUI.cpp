#include "pch.h"
#include "TextureUI.h"

TextureUI::TextureUI()
    : ResourceUI(RES_TYPE::TEXTURE)
{
    SetName("Texture");
}

TextureUI::~TextureUI()
{
}

int TextureUI::render_update()
{
    ResourceUI::render_update();
    return 0;
}
