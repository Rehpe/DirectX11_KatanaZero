#include "pch.h"
#include "CEffectScript.h"

#include <Engine\CAnim2D.h>
#include <Engine\CEngine.h>
#include <Engine\CLevel.h>

CEffectScript::CEffectScript()
	: CScript((UINT)SCRIPT_TYPE::EFFECTSCRIPT)
{
}

CEffectScript::~CEffectScript()
{
}

void CEffectScript::begin()
{
	if (GetOwner()->Animator2D())
		GetOwner()->Animator2D()->SetRepeat(false);

	switch (m_EffectType)
	{
	case (UINT)EffectType::DUSTCLOUD:
		break;
	case (UINT)EffectType::JUMPCLOUD:
		break;
	case (UINT)EffectType::LANDCLOUD:
		break;
	case (UINT)EffectType::BLOOD:
		break;
	case (UINT)EffectType::LONGSLASH:
	{
		// Slash Sprite 회전
		float fAngle = atan2(m_vDir.y, m_vDir.x);
		GetOwner()->Transform()->SetRelativeRot(Vec3(0.f, 0.f, fAngle));
		GetOwner()->Transform()->SetRelativeScale(Vec3(600.f, 50.f, 0.f));

		// 시작점
		m_vPos; // EnemyPos;
		CLevel* Level = CLevelManager::GetInst()->GetCurLevel();
		CGameObject* pCamera = Level->FindObjectByName(L"MainCamera");
		Vec3 CameraPos = pCamera->Transform()->GetRelativePos();
		Vec2 vResolution = CEngine::GetInst()->GetWindowResolution();

		float offScreenDistance = 500.f; // Adjust this value as needed

   // Calculate the starting point by subtracting the offScreenDistance in the attack direction from the strike point
		Vec3 startPos = m_vPos - (m_vDir * offScreenDistance);
		GetOwner()->Transform()->SetRelativePos(startPos);

		// 수명
		GetOwner()->SetLifeSpan(5.f);
	}
	break;
	}
}

void CEffectScript::tick()
{
	switch (m_EffectType)
	{
	case (UINT)EffectType::DUSTCLOUD:
		break;
	case (UINT)EffectType::JUMPCLOUD:
		break;
	case (UINT)EffectType::LANDCLOUD:
		break;
	case (UINT)EffectType::BLOOD:
		break;
	case (UINT)EffectType::LONGSLASH:
	{
		// AttackDir로 화면 끝에서 끝으로 이동
		float EffectSpeed = 5000.f;
		Vec3 pos = GetOwner()->Transform()->GetRelativePos();
		Vec3 m_Vec3Dir = Vec3(m_vDir.x, m_vDir.y, 0.f);
		pos += m_Vec3Dir * EffectSpeed * DT * 2.f;
		GetOwner()->Transform()->SetRelativePos(pos);

		// Color overlay (pink - blue)
		m_fTimer += DT;

		if (m_fTimer <= 0.1f)
		{
			float t = m_fTimer / 0.1f;
			XMVECTOR startColor = XMVectorSet(1.0f, 0.0f, 1.0f, 1.0f); // Neon pink
			XMVECTOR endColor = XMVectorSet(0.0f, 0.5f, 1.0f, 0.5f);   // Neon blue
			Vec4 currentColor = Vec4(XMVectorLerp(startColor,endColor, t));
			int OverlayUse = 1;
			float OverlayAlpha = 0.7f;
			GetOwner()->MeshRender()->GetMaterial()->SetScalarParam(INT_1, &OverlayUse);
			GetOwner()->MeshRender()->GetMaterial()->SetScalarParam(VEC4_0, &currentColor);
			GetOwner()->MeshRender()->GetMaterial()->SetScalarParam(FLOAT_0, &OverlayAlpha);
		}
	}
	break;
	}

	if (GetOwner()->Animator2D())
	{
		if (GetOwner()->Animator2D()->GetCurAnim()->IsFinish())
		{
			Destroy();
		}
	}
}
