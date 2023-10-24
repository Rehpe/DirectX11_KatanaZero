#pragma once
#include "ComponentUI.h"
class Light2DUI :
    public ComponentUI
{
public:
    Light2DUI();
    ~Light2DUI();

private:

public:
    virtual void init() override {};
    virtual int render_update() override;
};

