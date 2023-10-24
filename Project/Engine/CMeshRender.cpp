#include "pch.h"
#include "CMeshRender.h"

#include "CGraphicsShader.h"
#include "CMesh.h"

#include "CTransform.h"
#include "CAnimator2D.h"

CMeshRender::CMeshRender()
	: CRenderComponent(COMPONENT_TYPE::MESHRENDER)
{
}

CMeshRender::~CMeshRender()
{
}

void CMeshRender::finaltick()
{
}

void CMeshRender::render()
{
	// 둘 중 하나라도 없으면 렌더 불가능
	if (nullptr == GetMesh() || nullptr == GetMaterial())
		return;

	// Transform 에 UpdateData 요청
	Transform()->UpdateData();

	// Animator2D 컴포넌트가 있다면
	if (Animator2D())
	{
		Animator2D()->UpdateData();
	}

	// 재질 업데이트
	GetMaterial()->UpdateData();

	// 렌더
	GetMesh()->render();

	// Animation 관련 정보 제거
	if (Animator2D())
		Animator2D()->Clear();
}
