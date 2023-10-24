#pragma once
#include <Engine\CScript.h>

class CMonsterScript :
    public CScript
{
public:
    CMonsterScript();
    ~CMonsterScript();

public:
    CLONE(CMonsterScript);

public:
    virtual void tick() override;

public:
    virtual void BeginOverlap(CCollider2D* _Other) override;

};

