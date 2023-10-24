#pragma once
#include <Engine\CScript.h>

class CGruntAttackSpriteScript :
	public CScript
{
public:
	CGruntAttackSpriteScript();
	~CGruntAttackSpriteScript();

public:
	CLONE(CGruntAttackSpriteScript);

private:
	bool	m_bActivate;

public:
	virtual void tick() override;

public:
	void SetActive(bool _b) { m_bActivate = _b; }
public:
	virtual void BeginOverlap(CCollider2D* _Other) override;
	virtual void OnOverlap(CCollider2D* _Other) override;
	virtual void EndOverlap(CCollider2D* _Other) override;

};

