#include "pch.h"
#include "CCursorScript.h"

#include <Engine\CKeyManager.h>
#include <Engine\CDevice.h>

CCursorScript::CCursorScript()
	: CScript((UINT)SCRIPT_TYPE::CURSORSCRIPT)
{
}

CCursorScript::~CCursorScript()
{
}


void CCursorScript::tick()
{
	Vec2 Resolution = CDevice::GetInst()->GetRenderResolution();
	Vec2 CursorPos = CKeyManager::GetInst()->GetMousePos();
	Vec2 halfScreenResolution = Resolution * 0.5f;
	CursorPos = Vec2((float)CursorPos.x, (float)CursorPos.y) - halfScreenResolution;
	Vec3 Pos = Vec3(CursorPos.x, -CursorPos.y, 0.f);
	GetOwner()->Transform()->SetRelativePos(Pos);
}

