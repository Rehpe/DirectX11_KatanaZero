#pragma once
#include "ComponentUI.h"
class MeshRenderUI :
    public ComponentUI
{
public:
    MeshRenderUI();
    ~MeshRenderUI();

public:
    virtual void init() override {};
    virtual int render_update() override;

public:
    void SelectMesh(DWORD_PTR _Key);
    void SelectMaterial(DWORD_PTR _Key);


};

