#include "pch.h"
#include "CLaserSwitchScript.h"

#include <Engine\CLayer.h>
#include <Engine\CLevel.h>

#include "CLightFlickerScript.h"

CLaserSwitchScript::CLaserSwitchScript()
	: CScript((UINT)SCRIPT_TYPE::LASERSWITCHSCRIPT)
	, m_Lasers{}
	, m_bActive(true)
{
}

CLaserSwitchScript::~CLaserSwitchScript()
{
}


void CLaserSwitchScript::begin()
{
	// 레벨 내의 모든 레이저 초기화
	CLevel* Level = CLevelManager::GetInst()->GetCurLevel();
	m_Lasers = Level->FindLayerByName(L"Laser")->GetParentObject();

	m_bActive = true;
}

void CLaserSwitchScript::tick()
{

	if (m_bActive)
	{
		GetOwner()->MeshRender()->GetMaterial()->SetTexParam(TEX_0, CResourceManager::GetInst()->FindRes<CTexture>(L"texture\\map\\SwitchOn.png"));

		// 레이저 ON
		for (size_t i = 0; i < m_Lasers.size(); i++)
		{
			CGameObject* Laser = m_Lasers[i];
			Laser->Collider2D()->SetColliderActivate(true);
			Laser->MeshRender()->SetMaterial(CResourceManager::GetInst()->FindRes<CMaterial>(L"material\\LaserMtrl.mtrl"));
		}

		// PointLight Red
		CLevel* Level = CLevelManager::GetInst()->GetCurLevel();
		vector<CGameObject*> PointLights;
		Level->FindObjectByName(L"LaserPointLight", PointLights);
		
		for (size_t i = 0; i < PointLights.size(); i++)
		{
			CLightFlickerScript* Flicker = PointLights[i]->GetScript<CLightFlickerScript>();
			Flicker->ResetDiffuse();
		}
	}

	
	else
	{
		GetOwner()->MeshRender()->GetMaterial()->SetTexParam(TEX_0, CResourceManager::GetInst()->FindRes<CTexture>(L"texture\\map\\SwitchOff.png"));

		// 레이저 OFF
		for (size_t i = 0; i < m_Lasers.size(); i++)
		{
			CGameObject* Laser = m_Lasers[i];
			Laser->Collider2D()->SetColliderActivate(false);
			Laser->MeshRender()->SetMaterial(nullptr);
		}

		// PointLight Green
		CLevel* Level = CLevelManager::GetInst()->GetCurLevel();
		vector<CGameObject*> PointLights;
		Level->FindObjectByName(L"LaserPointLight", PointLights);

		for (size_t i = 0; i < PointLights.size(); i++)
		{
			CLightFlickerScript* Flicker = PointLights[i]->GetScript<CLightFlickerScript>();
			Flicker->SetCurDiffuse(Vec3(0.17f, 1.f, 0.f));
		}
	}
}

void CLaserSwitchScript::OnOverlap(CCollider2D* _Other)
{
	if (L"Player" == _Other->GetOwner()->GetName())
	{
		if (KEY_TAP(KEY::SPACE))
		{
			if (m_bActive)
			{
				m_bActive = false;
				// Sound Effect
				Ptr<CSound> LaserOff = CResourceManager::GetInst()->FindRes<CSound>(L"sound\\SFX\\TurnOffLaser.wav");
				LaserOff->Play(1, CTimeManager::GetInst()->GetTimeFactor(), 0.5f, false);
			}
			else
				m_bActive = true;
		}
	}
}
