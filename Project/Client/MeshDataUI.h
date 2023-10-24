#pragma once
#include "ResourceUI.h"
class MeshDataUI :
    public ResourceUI
{
public:
    MeshDataUI();
    ~MeshDataUI();

public:
    virtual int render_update() override;
};

