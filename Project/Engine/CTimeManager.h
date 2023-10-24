#pragma once


class CTimeManager
	: public CSingleton<CTimeManager>
{	
	SINGLETON(CTimeManager);

private:
	LARGE_INTEGER	m_llPrevCount;
	LARGE_INTEGER	m_llCurCount;
	LARGE_INTEGER	m_llFrequency;

	UINT			m_iCallCount;
	float			m_fDeltaTime;
	float			m_fTime;
	float			m_fTimeFactor;

public:
	void init();
	void tick();
	void render();

public:
	float GetDeltaTime() { return m_fDeltaTime; }
	float GetTimeFactor() { return m_fTimeFactor; }
	void SetTimeFactor(float _f) { m_fTimeFactor = _f; }
	void ResetTimeFactor() { m_fTimeFactor = 1.f; }
};

