#include "pch.h"
#include "CMissileScript.h"

CMissileScript::CMissileScript()
	: CScript((UINT)SCRIPT_TYPE::MISSILESCRIPT)
	, m_fSpeed(0.f)
	, m_vDir(Vec3(0.f, 1.f, 0.f))		// À§(UP) ¹æÇâ
{
}

CMissileScript::~CMissileScript()
{
}

void CMissileScript::tick()
{
	Vec3 vPos = Transform()->GetRelativePos();

	vPos += m_vDir * DT * m_fSpeed;

	Transform()->SetRelativePos(vPos);
}
