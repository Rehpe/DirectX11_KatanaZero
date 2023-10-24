#pragma once
#include "ResourceUI.h"
class MeshUI :
    public ResourceUI
{
public:
    MeshUI();
    ~MeshUI();

public:
    virtual int render_update() override;
};

