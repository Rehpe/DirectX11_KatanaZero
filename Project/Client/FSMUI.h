#pragma once
#include "ComponentUI.h"
class FSMUI :
    public ComponentUI
{
public:
    FSMUI();
    ~FSMUI();

private:

public:
    virtual void init() override;
    virtual int render_update() override;
};

