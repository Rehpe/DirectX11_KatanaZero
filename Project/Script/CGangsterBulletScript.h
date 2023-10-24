#pragma once
#include <Engine\CScript.h>

class CGangsterBulletScript :
    public CScript
{
public:
	CGangsterBulletScript();
	~CGangsterBulletScript();

public:
	CLONE(CGangsterBulletScript);

private:
	CGameObject*	m_pShooter;
	float			m_fBulletSpeed;
	Vec3			m_vTargetDir;
	bool			m_bReflect;
	Vec3			m_vReflectDir;

public:
	virtual void begin() override;
	virtual void tick() override;

public:
	virtual void BeginOverlap(CCollider2D* _Other) override;
	virtual void OnOverlap(CCollider2D* _Other) override;
	virtual void EndOverlap(CCollider2D* _Other) override;

public:
	void SetShooter(CGameObject* _Gangster) { m_pShooter = _Gangster; }
	void CalcReflectDir(CCollider2D* _Attack);
	void Reflect();
};

