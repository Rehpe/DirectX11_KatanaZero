#pragma once
#include <Engine\CScript.h>

class CLaserScript :
    public CScript
{
public:
    CLaserScript();
    ~CLaserScript();

public:
    CLONE(CLaserScript);

private:

public:
    virtual void begin() override;
    virtual void tick() override;

public:
    virtual void BeginOverlap(CCollider2D* _Other);
    virtual void OnOverlap(CCollider2D* _Other);
};

