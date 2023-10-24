#include "pch.h"
#include "CHudScript.h"

#include <Engine\CLevel.h>
#include "CGameManagerScript.h"
#include <Engine\CRecordManager.h>

CHudScript::CHudScript()
	: CScript((UINT)SCRIPT_TYPE::HUDSCRIPT)
	, m_arrBattery{}
	, m_bInit(false)
	, m_GMScript(nullptr)
	, m_Timer(nullptr)
{
}

CHudScript::~CHudScript()
{
}

void CHudScript::begin()
{
	m_bInit = true;
}

void CHudScript::tick()
{
	if (m_bInit)
		init();

	if (m_GMScript == nullptr)
		return;

	if (CRecordManager::GetInst()->GetPlayMode() != (UINT)PLAYMODE::NONE)
		return;

	// 게임 진행 시간 반영
	float MaxTimeLimit = m_GMScript->GetMaxTimeLimit();
	float RemainingTime = m_GMScript->GetTimer();
	float Percentage = RemainingTime / MaxTimeLimit;

	m_Timer->MeshRender()->GetMaterial()->SetScalarParam(FLOAT_0, &Percentage);

	// SlowMode CoolDown 반영
	float SlowModeCoolDown = m_GMScript->GetSlowModeCoolDown();
	float SlowModeTimer = m_GMScript->GetSlowModeTimer();

	int lastVisibleIndex = static_cast<int>(std::ceil(SlowModeTimer)) - 1;
	for (int i = 0; i < m_arrBattery.size(); ++i)
	{
		if (i <= lastVisibleIndex)
		{
			// 보인다
			m_arrBattery[i]->MeshRender()->SetMaterial(CResourceManager::GetInst()->FindRes<CMaterial>(L"material\\UI_BatteryMtrl.mtrl"));
		}
		else
		{
			// 안보인다
			m_arrBattery[i]->MeshRender()->SetMaterial(nullptr);
		}
	}
}

void CHudScript::init()
{
	// GM Script 초기화
	CLevel* CurLevel = CLevelManager::GetInst()->GetCurLevel();
	CGameObject* m_GM = CurLevel->FindObjectByName(L"GameManager");
	if (m_GM == nullptr)
		return;

	m_GMScript = m_GM->GetScript<CGameManagerScript>();

	// Timer 초기화
	m_Timer = GetOwner()->GetChildObjByName(L"Timer");
	int UseTimerShader = 1;
	m_Timer->MeshRender()->GetMaterial()->SetScalarParam(INT_0, &UseTimerShader);
	float Init = 1.f;
	m_Timer->MeshRender()->GetMaterial()->SetScalarParam(FLOAT_0, &Init);

	// Battery Prefab 생성
	Vec3 FirstPos = Vec3(-612.f, 330.f, 7.f);
	Ptr<CPrefab> Battery = CResourceManager::GetInst()->FindRes<CPrefab>(L"prefab\\Battery.pref");

	for (size_t i = 0; i < 11; i++)
	{
		m_arrBattery.push_back(Battery->Instantiate());
		Vec3 Pos = Vec3(FirstPos.x + 10.f * i, FirstPos.y, 7.f);
		SpawnGameObject(m_arrBattery[i], Pos, L"UI");
	}

	m_bInit = false;
}
