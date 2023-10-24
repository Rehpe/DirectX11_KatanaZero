#pragma once
#include <Engine\CScript.h>

class CRampScript :
    public CScript
{
public:
    CRampScript();
    ~CRampScript();

public:
    CLONE(CRampScript);

public:
    virtual void tick() override;

public:
    virtual void BeginOverlap(CCollider2D* _Other) override;
    virtual void OnOverlap(CCollider2D* _Other) override;
    virtual void EndOverlap(CCollider2D* _Other) override;

    void ResolveCollision(CCollider2D* _colliderA, CCollider2D* _colliderB);
};

