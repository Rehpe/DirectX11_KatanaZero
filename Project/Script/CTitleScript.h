#pragma once
#include <Engine\CScript.h>

class CTitleScript :
    public CScript
{
public:
    CTitleScript();
    ~CTitleScript();

public:
    CLONE(CTitleScript);

public:
    virtual void begin() override;
    virtual void tick() override;
};

