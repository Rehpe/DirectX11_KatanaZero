#pragma once
#include <Engine\CScript.h>

class CGroundScript :
    public CScript
{
public:
    CGroundScript();
    ~CGroundScript();

public:
    CLONE(CGroundScript);

private:

public:
    virtual void begin() override;
    virtual void tick() override;

public:
    virtual void BeginOverlap(CCollider2D* _Other);
    virtual void OnOverlap(CCollider2D* _Other);
    virtual void EndOverlap(CCollider2D* _Other);

public:
    void AntiOverlap(CCollider2D* _ObjCol);
    //Vec3 CalculateOverlapAmount(CCollider2D* _colliderA, CCollider2D* _colliderB);



};

