#pragma once
#include <Engine\CScript.h>

class COneWayPlatformScript :
    public CScript
{
public:
    COneWayPlatformScript();
    ~COneWayPlatformScript();

public:
    CLONE(COneWayPlatformScript);

public:
    virtual void tick() override;

public:
    virtual void BeginOverlap(CCollider2D* _Other) override;
    virtual void OnOverlap(CCollider2D* _Other) override;
    virtual void EndOverlap(CCollider2D* _Other) override;

public:
    //Vec3 CalculateOverlapAmount(CCollider2D* _colliderA, CCollider2D* _colliderB);
    void AntiOverlap(CCollider2D* _ObjCol);

};

