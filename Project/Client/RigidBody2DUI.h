#pragma once
#include "ComponentUI.h"
class RigidBody2DUI :
    public ComponentUI
{
public:
    RigidBody2DUI();
    ~RigidBody2DUI();

private:

public:
    virtual void init() override;
    virtual int render_update() override;
};

