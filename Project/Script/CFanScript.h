#pragma once
#include <Engine\CScript.h>

class CFanScript :
    public CScript
{

public:
    CFanScript();
    ~CFanScript();

public:
    CLONE(CFanScript);

public:
    virtual void begin();
    virtual void tick();

public:
    virtual void OnOverlap(CCollider2D* _Other);

};

