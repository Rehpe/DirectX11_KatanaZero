#pragma once
#include "CEnemyScript.h"
class CEnemyGangsterScript :
    public CEnemyScript
{
public:
	CEnemyGangsterScript();
	~CEnemyGangsterScript();

public:
	CLONE(CEnemyGangsterScript);

private:
	float	m_fShootTimer;
	float	m_fShootCoolDown;
	bool	m_bIsShootable;

public:
	virtual void begin() override;
	virtual void tick() override;

public:
	bool IsShootable() { return m_bIsShootable; }
	void SetShootable(bool _b) { m_bIsShootable = _b; }

public:
	virtual void BeginOverlap(CCollider2D* _Other) override;
	virtual void OnOverlap(CCollider2D* _Other) override;
	virtual void EndOverlap(CCollider2D* _Other) override;

public:
	virtual void SaveToLevelFile(FILE* _File) override;
	virtual void LoadFromLevelFile(FILE* _File) override;
};

