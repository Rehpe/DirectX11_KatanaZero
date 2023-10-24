#pragma once
#include  <Engine\CScript.h>
class CAttackSpriteScript :
    public CScript
{
public:
    CAttackSpriteScript();
    ~CAttackSpriteScript();

public:
    CLONE(CAttackSpriteScript);

private:

public:
    virtual void tick() override;

public:
    virtual void BeginOverlap(CCollider2D* _Other);
    //virtual void OnOverlap(CCollider2D* _Other);
    //virtual void EndOverlap(CCollider2D* _Other);
};

