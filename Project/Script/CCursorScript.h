#pragma once
#include <Engine\CScript.h>
class CCursorScript :
    public CScript
{
public:
    CCursorScript();
    ~CCursorScript();

public:
    CLONE(CCursorScript);

public:
    virtual void tick() override;
};

