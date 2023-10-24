#pragma once
#include <Engine\CScript.h>

class CWallScript :
    public CScript
{
public:
    CWallScript();
    ~CWallScript();

public:
    CLONE(CWallScript);

private:
    UINT    m_WallType;
    bool    m_bIsEndWall;

public:
    virtual void tick() override;

public:
    virtual void BeginOverlap(CCollider2D* _Other) override;
    virtual void OnOverlap(CCollider2D* _Other) override;
    virtual void EndOverlap(CCollider2D* _Other) override;

public:
    //Vec3 CalculateOverlapAmount(CCollider2D* _colliderA, CCollider2D* _colliderB);
    UINT GetWallType() { return m_WallType; }
    bool   IsEndWall() { return m_bIsEndWall; }

public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
};

