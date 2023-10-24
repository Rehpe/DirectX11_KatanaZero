#include "pch.h"
#include "CCameraScript.h"

#include <Engine\CLevel.h>
#include <cmath>

CCameraScript::CCameraScript()
	: CScript((UINT)SCRIPT_TYPE::CAMERASCRIPT)
	,m_vCameraOffset(Vec3(0.f, 50.f, 0))
	,m_fSmoothFactor(5.f)
	, m_bShakeCamera(false)
	, m_fShakeDuration(0.f)
	, m_fShakeMagnitude(0.f)
	, m_fShakeTimer(0.f)
{
}

CCameraScript::~CCameraScript()
{
}


void CCameraScript::begin()
{

}

void CCameraScript::tick()
{
	InitFramePos();

	m_vCurrentPos = GetOwner()->Transform()->GetRelativePos();
	CLevel* Level = CLevelManager::GetInst()->GetCurLevel();
	CGameObject* Player = Level->FindObjectByName(L"Player");

	if (Player == nullptr)
		return;

	m_vPlayerPos = Player->Transform()->GetRelativePos();

	Vec3 targetPos = m_vPlayerPos + m_vCameraOffset;

	// Apply constraints (optional)
	targetPos.x = clamp(targetPos.x, m_vMinPos.x, m_vMaxPos.x);
	targetPos.y = clamp(targetPos.y, m_vMinPos.y, m_vMaxPos.y);

	m_vCurrentPos = lerp(m_vCurrentPos, targetPos, m_fSmoothFactor * DT);

	// Camera Shake ½Ã
	if (m_bShakeCamera)
	{
		static Vec2 ShakeDir = Vec2(1.f, 1.f);

		float shakeProgress = m_fShakeTimer / m_fShakeDuration;
		shakeProgress = 1 - (1 - shakeProgress) * (1 - shakeProgress);
		m_fShakeMagnitude = m_fShakeMagnitude * (1 - shakeProgress);

		Vec2 ShakeRange = Vec2(0.7f, 0.3f);

		float x = ShakeRange.x * m_fShakeMagnitude * ShakeDir.x;
		float y = ShakeRange.y * m_fShakeMagnitude * ShakeDir.y;

		m_vCurrentPos.x += x;
		m_vCurrentPos.y += y;

		ShakeDir = -1 * ShakeDir;

		m_fShakeTimer += DT;
		
		if (m_fShakeTimer >= m_fShakeDuration)
		{
			m_bShakeCamera = false;
		}
	}
	GetOwner()->Transform()->SetRelativePos(m_vCurrentPos);
}

void CCameraScript::InitFramePos()
{
	CLevel* Level = CLevelManager::GetInst()->GetCurLevel();
	if (Level->GetName() == L"Level1")
	{
		m_vMinPos = Vec2(5.f, 0.f);
		m_vMaxPos = Vec2(480.f, 0.f);
	}
	else if (Level->GetName() == L"Level2")
	{
		m_vMinPos = Vec2(-70.f, -200.f);
		m_vMaxPos = Vec2(520.f, 220.f);
	}
	else if (Level->GetName() == L"Level3")
	{
		m_vMinPos = Vec2(-90.f, 90.f);
		m_vMaxPos = Vec2(670.f, 400.f);
	}
	
}

void CCameraScript::ApplyCameraShake(float Duration, float Magnitude)
{
	m_fShakeDuration = Duration;
	m_fShakeMagnitude = Magnitude;
	m_fShakeTimer = 0.0f;
	m_bShakeCamera = true;
}
