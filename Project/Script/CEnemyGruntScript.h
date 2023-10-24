#pragma once
#include "CEnemyScript.h"
class CEnemyGruntScript :
    public CEnemyScript
{
public:
	CEnemyGruntScript();
	~CEnemyGruntScript();

public:
	CLONE(CEnemyGruntScript);

private:
	
public:
	virtual void begin() override;
	virtual void tick() override;

public:
	virtual void BeginOverlap(CCollider2D* _Other) override;
	virtual void OnOverlap(CCollider2D* _Other) override;
	virtual void EndOverlap(CCollider2D* _Other) override;

public:
	virtual void SaveToLevelFile(FILE* _File) override;
	virtual void LoadFromLevelFile(FILE* _File) override;
};

