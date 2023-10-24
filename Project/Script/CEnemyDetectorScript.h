#pragma once
#include <Engine\CScript.h>

class CEnemyDetectorScript :
    public CScript
{
public:
	CEnemyDetectorScript();
	~CEnemyDetectorScript();

public:
	CLONE(CEnemyDetectorScript);

private:
	float	m_fDetectRange;

public:
	virtual void begin() override;
	virtual void tick() override;

public:
	virtual void BeginOverlap(CCollider2D* _Other) override;
	virtual void OnOverlap(CCollider2D* _Other) override;
	virtual void EndOverlap(CCollider2D* _Other) override;

};

