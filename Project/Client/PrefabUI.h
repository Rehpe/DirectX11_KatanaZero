#pragma once
#include "ResourceUI.h"
class PrefabUI :
    public ResourceUI
{
public:
    PrefabUI();
    ~PrefabUI();

public:
    virtual int render_update() override;
};

