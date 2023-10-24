#include "pch.h"
#include "CLight2D.h"

#include "CRenderManager.h"
#include "CTransform.h"

CLight2D::CLight2D()
	: CComponent(COMPONENT_TYPE::LIGHT2D)
{
}

CLight2D::~CLight2D()
{
}

void CLight2D::finaltick()
{
	m_LightInfo.vWorldPos = Transform()->GetWorldPos();
	m_LightInfo.vWorldDir = Transform()->GetWorldDir(DIR_TYPE::RIGHT);

	// Light2D는 매 finaltick마다 광원 정보를 RenderManager에 등록시킨다.
	CRenderManager::GetInst()->RegisterLight2D(m_LightInfo);
}

void CLight2D::SaveToLevelFile(FILE* _File)
{
	// LightInfo 구조체 통째로 저장
	fwrite(&m_LightInfo, sizeof(tLightInfo), 1, _File);
}

void CLight2D::LoadFromLevelFile(FILE* _File)
{
	fread(&m_LightInfo, sizeof(tLightInfo), 1, _File);
}
