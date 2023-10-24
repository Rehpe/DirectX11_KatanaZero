#pragma once
#include "ResourceUI.h"
class SoundUI :
    public ResourceUI
{
public:
    SoundUI();
    ~SoundUI();

public:
    virtual int render_update() override;
};

