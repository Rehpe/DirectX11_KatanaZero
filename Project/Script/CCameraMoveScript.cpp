#include "pch.h"
#include "CCameraMoveScript.h"

#include <Engine\CCamera.h>

CCameraMoveScript::CCameraMoveScript()
	: CScript((UINT)SCRIPT_TYPE::CAMERAMOVESCRIPT)
	, m_fCamSpeed(100.f) 
{
}

CCameraMoveScript::~CCameraMoveScript()
{
}

void CCameraMoveScript::tick()
{
	// ���������Ͻ� 2d ī�޶� Move
	if (PROJ_TYPE::ORTHOGRAPHIC == Camera()->GetProjType())
		Camera2DMove();
	// ���������Ͻ� 3d ī�޶� Move
	else
		Camera3DMove();
}

void CCameraMoveScript::Camera2DMove()
{
	// ����Ű �Է¿� ���� ī�޶� �̵�
	Vec3 vPos = Transform()->GetRelativePos();

	if (KEY_PRESSED(KEY::W))
	{
		vPos.y += DT * m_fCamSpeed;
	}

	if (KEY_PRESSED(KEY::S))
	{
		vPos.y -= DT * m_fCamSpeed;
	}

	if (KEY_PRESSED(KEY::A))
	{
		vPos.x -= DT * m_fCamSpeed;
	}

	if (KEY_PRESSED(KEY::D))
	{
		vPos.x += DT * m_fCamSpeed;
	}

	if (KEY_PRESSED(KEY::_1))
	{
		float fScale = Camera()->GetScale();
		fScale += DT * 1.f;
		Camera()->SetScale(fScale);
	}

	if (KEY_PRESSED(KEY::_2))
	{
		float fScale = Camera()->GetScale();
		fScale -= DT * 1.f;
		Camera()->SetScale(fScale);
	}

	Transform()->SetRelativePos(vPos);
}

void CCameraMoveScript::Camera3DMove()
{
	Vec3 vPos = Transform()->GetRelativePos();
	Vec3 vRot = Transform()->GetRelativeRot();
	Vec3 vFront = Transform()->GetRelativeDir(DIR_TYPE::FRONT);
	Vec3 vUp = Transform()->GetRelativeDir(DIR_TYPE::UP);
	Vec3 vRight = Transform()->GetRelativeDir(DIR_TYPE::RIGHT);

	if (KEY_PRESSED(KEY::W))
	{
		vPos += DT * vFront * m_fCamSpeed;
	}

	if (KEY_PRESSED(KEY::S))
	{
		vPos -= DT * vFront * m_fCamSpeed;
	}

	if (KEY_PRESSED(KEY::A))
	{
		vPos -= DT * vRight * m_fCamSpeed;
	}

	if (KEY_PRESSED(KEY::D))
	{
		vPos += DT * vRight * m_fCamSpeed;
	}

	if (KEY_PRESSED(KEY::RBTN))
	{
		Vec2 vMouseDir = CKeyManager::GetInst()->GetMouseDir();

		vRot.y += DT * vMouseDir.x;		// ���콺�� ��/���������� ���ȴٸ� ������ y������ �������� ���� �� (Yaw)
		vRot.x -= DT * vMouseDir.y;		// ���콺�� ��/�Ʒ������� ���ȴٸ� ������ x������ �������� ���� �� (Pitch)
	}

	Transform()->SetRelativePos(vPos);
	Transform()->SetRelativeRot(vRot);
}
