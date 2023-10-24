#include "pch.h"
#include "SoundUI.h"

SoundUI::SoundUI()
    : ResourceUI(RES_TYPE::SOUND)
{
    SetName("Sound");
}

SoundUI::~SoundUI()
{
}

int SoundUI::render_update()
{
    ResourceUI::render_update();
    return 0;
}
