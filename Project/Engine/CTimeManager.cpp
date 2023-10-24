#include "pch.h"
#include "CTimeManager.h"

#include "CEngine.h"

#include <chrono>
#include <thread>

const int forceFrameRate = 120;

CTimeManager::CTimeManager()
	: m_llPrevCount{}
	, m_llCurCount{}
	, m_llFrequency{}
	, m_iCallCount(0)
	, m_fDeltaTime(0.f)
	, m_fTime(0.f)
	, m_fTimeFactor(1.f)
{

}

CTimeManager::~CTimeManager()
{

}


void CTimeManager::init()
{
	// 1초당 카운팅 증가량
	QueryPerformanceFrequency(&m_llFrequency);

	QueryPerformanceCounter(&m_llCurCount);
	QueryPerformanceCounter(&m_llPrevCount);

}

void CTimeManager::tick()
{
	float TargetDT = 1.f / forceFrameRate;

	QueryPerformanceCounter(&m_llCurCount);

	// tick 간격 시간
	m_fDeltaTime = (float)(m_llCurCount.QuadPart - m_llPrevCount.QuadPart) / (float)m_llFrequency.QuadPart;

	while (m_fDeltaTime < TargetDT)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		QueryPerformanceCounter(&m_llCurCount);
		m_fDeltaTime = (float)(m_llCurCount.QuadPart - m_llPrevCount.QuadPart) / (float)m_llFrequency.QuadPart;
	}

	m_fDeltaTime *= m_fTimeFactor;

	// 누적 시간
	m_fTime += m_fDeltaTime;

	// 함수 호출 횟수
	++m_iCallCount;

	// 이전 카운트 값을 현재 카운트로 갱신
	m_llPrevCount = m_llCurCount;

	// GlobalData 갱신
	GlobalData.tDT = m_fDeltaTime;
	GlobalData.tAccTime += m_fDeltaTime;
}

void CTimeManager::render()
{
	// 1초에 한번
	if (1.f <= m_fTime)
	{
		wchar_t szBuff[256] = {};
		swprintf_s(szBuff, L"FPS : %d, DT : %f", m_iCallCount, m_fDeltaTime);
		SetWindowText(CEngine::GetInst()->GetMainWnd(), szBuff);

		m_fTime = 0.f;
		m_iCallCount = 0;
	}
}
