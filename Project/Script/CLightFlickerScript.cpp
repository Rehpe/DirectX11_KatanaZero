#include "pch.h"
#include "CLightFlickerScript.h"
#include <Engine\CRecordManager.h>

CLightFlickerScript::CLightFlickerScript()
	: CScript((UINT)SCRIPT_TYPE::LIGHTFLICKERSCRIPT)
	, m_fSpeed(1.f)
	, m_fTimer(0.f)
{
    
}

CLightFlickerScript::~CLightFlickerScript()
{
}


void CLightFlickerScript::begin()
{
    m_fMaxIntensity = GetOwner()->Light2D()->GetIntensity();
    m_fOriginalDiffuse = GetOwner()->Light2D()->GetLightDiffuse();
    m_fCurDiffuse = m_fOriginalDiffuse;
}

void CLightFlickerScript::tick()
{
    if (CRecordManager::GetInst()->GetPlayMode() != (UINT)PLAYMODE::NONE)
        return;

    m_fTimer += DT;

    m_fSpeed = 3.f;

    float MinIntensity = m_fMaxIntensity * 0.5f;
    float intensity = MinIntensity + (m_fMaxIntensity - MinIntensity) * (1.0f + sinf(m_fTimer * m_fSpeed)) / 2.0f;

    GetOwner()->Light2D()->SetLightDiffuse(m_fCurDiffuse * intensity);
}
