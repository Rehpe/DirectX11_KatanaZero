#pragma once
#include "ComponentUI.h"
class CameraUI :
    public ComponentUI
{
public:
    CameraUI();
    ~CameraUI();

private:

public:
    virtual void init() override {};
    virtual int render_update() override;
};

