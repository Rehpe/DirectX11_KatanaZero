#pragma once
#include <Engine\CScript.h>

class CBreakableFloorScript :
    public CScript
{
public:
    CBreakableFloorScript();
    ~CBreakableFloorScript();

public:
    CLONE(CBreakableFloorScript);

public:
    virtual void tick() override;

public:
    //CollisionDir GetCollisionDir(CCollider2D* _colliderA, CCollider2D* _colliderB);
    //Vec3 CalculateOverlapAmount(CCollider2D* _colliderA, CCollider2D* _colliderB);

public:
    virtual void BeginOverlap(CCollider2D* _Other) override;
    virtual void OnOverlap(CCollider2D* _Other) override;
    virtual void EndOverlap(CCollider2D* _Other) override;



};

