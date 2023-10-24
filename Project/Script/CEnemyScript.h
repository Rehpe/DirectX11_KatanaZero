#pragma once
#include <Engine\CScript.h>

class CEnemyScript :
	public CScript
{
protected:
	CEnemyScript(UINT ScriptType);

public:
	CEnemyScript();
	virtual ~CEnemyScript();

public:
	CLONE(CEnemyScript);

protected:
	Vec3	m_vSpawnPos;
	int		m_iNoticedDoorIdx;		// 열리면 Notice상태가 되는 Door 번호

private:
	int		m_iLookDir;
	int		m_iHp;
	
	float	m_fWalkSpeed;
	float	m_fSpeed;

	bool	m_bNoticed;

	bool	m_bDead;
	bool	m_bGround;
	bool	m_bIsCollidingWithPlayer;
	bool	m_bIsDetectingPlayer;
	bool	m_bIsDetectingDoor;
	bool	m_bIsBlind;


	float	m_fAttackTimer;
	float	m_fAttackCoolDown;
	bool	m_bIsAttackable;
	bool	m_bIsUsingRamp;

	Vec3	m_vKnockBackDir;

	Vec3 m_vCollidingRampStartPos;
	Vec3 m_vCollidingRampEndPos;

public:
	virtual void begin() override;
	virtual void tick() override;

public:
	void SetSpawnPos(Vec3 _pos) { m_vSpawnPos = _pos; }
	Vec3 GetSpawnPos() { return m_vSpawnPos; }

	int GetLookDir() { return m_iLookDir; }
	void SetLookDir(int _iDir) { m_iLookDir = _iDir; }

	float GetWalkSpeed() { return m_fWalkSpeed; }
	void SetWalkSpeed(float _f) { m_fWalkSpeed = _f; }

	float GetRunSpeed() { return m_fSpeed; }
	void SetRunSpeed(float _f) { m_fSpeed = _f; }

	Vec3 GetCollidingRampStartPos() { return m_vCollidingRampStartPos; }
	Vec3 GetCollidingRampEndPos() { return m_vCollidingRampEndPos; }

	Vec3 GetKnockBackDir() { return m_vKnockBackDir; }

	void ReduceHp(int _i) { m_iHp -= _i; }

	int GetNoticedDoorIdx() { return m_iNoticedDoorIdx; }

public:
	void SetDead(bool _b) { m_bDead = _b; }
	bool IsDead() { return m_bDead; }

	void SetNoticed(bool _b) { m_bNoticed = _b; }
	bool IsNoticed() { return m_bNoticed; }

	void SetGround(bool _b) { m_bGround = _b; }

	bool IsAttackable() { return m_bIsAttackable; }
	void SetAttackable(bool _b) { m_bIsAttackable = _b; }

	bool IsCollidingWithPlayer() { return m_bIsCollidingWithPlayer; }
	bool IsDetectingDoor() { return m_bIsDetectingDoor; }
	void SetDetectingDoor(bool _b) { m_bIsDetectingDoor = _b; }

	bool IsDetectingPlayer() { return m_bIsDetectingPlayer; }
	void SetDetectingPlayer(bool _b) { m_bIsDetectingPlayer = _b; }

	bool IsUsingRamp() { return m_bIsUsingRamp; };
	void SetUsingRamp(bool _b) { m_bIsUsingRamp = _b; }

public:
	virtual void BeginOverlap(CCollider2D* _Other) override;
	virtual void OnOverlap(CCollider2D* _Other) override;
	virtual void EndOverlap(CCollider2D* _Other) override;

public:
	bool IsPlayerOnSameFloor();
	Vec3 FindNearestRampPos();
	void MoveAlongRamp();
	void CalcKnockBackDir(CCollider2D* _Other);

public:
	virtual void SaveToLevelFile(FILE* _File) override;
	virtual void LoadFromLevelFile(FILE* _File) override;

};

