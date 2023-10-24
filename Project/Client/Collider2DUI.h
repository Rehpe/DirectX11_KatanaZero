#pragma once
#include "ComponentUI.h"
class Collider2DUI :
    public ComponentUI
{
public:
    Collider2DUI();
    ~Collider2DUI();

private:

public:
    virtual void init() override {};
    virtual int render_update() override;
};

