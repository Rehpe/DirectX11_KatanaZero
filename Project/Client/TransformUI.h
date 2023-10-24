#pragma once
#include "ComponentUI.h"

class TransformUI :
    public ComponentUI
{
public:
    TransformUI();
    ~TransformUI();

public:
    virtual void init() override {};
    virtual int render_update() override;
};

