#include "pch.h"
#include "CWallScript.h"
#include "CPlayerScript.h"

CWallScript::CWallScript()
	:CScript((UINT)SCRIPT_TYPE::WALLSCRIPT)
	, m_WallType((UINT)WALL_TYPE::NONE)
{
	AddScriptParam(SCRIPT_PARAM::INT, &m_WallType,
		"WALL TYPE (1:BlockLeft 2: BlockRight 3:BlockTop 4:HangLeft 5: HangRight");
	AddScriptParam(SCRIPT_PARAM::INT, &m_bIsEndWall,
		"0 = False, 1 = True");
}

CWallScript::~CWallScript()
{
}

void CWallScript::tick()
{
}

void CWallScript::BeginOverlap(CCollider2D* _Other)
{
	
}

void CWallScript::OnOverlap(CCollider2D* _Other)
{
	if (L"Player" == _Other->GetOwner()->GetName())
	{
		CGameObject* Player = _Other->GetOwner();
		CPlayerScript* PlayerScript = Player->GetScript<CPlayerScript>();

		// 플레이어에게 본인의 벽 타입과 충돌 여부를 알려준다.
		PlayerScript->SetWallCollision(m_WallType, 1);

		// 충돌체 파묻히는 것 꺼내기
		Vec3 overlap = CalculateOverlapAmount(_Other, GetOwner()->Collider2D());
		float threshhold = 1.f;

		// Push the player out of the ground
		Vec3 playerPos = Player->Transform()->GetRelativePos();
		if (overlap.x >= threshhold)
		{
			if (m_WallType == (UINT)WALL_TYPE::BLOCKWALL_LEFT
				|| m_WallType == (UINT)WALL_TYPE::HANGABLEWALL_LEFT)
			{
				playerPos.x += overlap.x;
			}
			if (m_WallType == (UINT)WALL_TYPE::BLOCKWALL_RIGHT
				|| m_WallType == (UINT)WALL_TYPE::HANGABLEWALL_RIGHT)
			{
				playerPos.x -= overlap.x;
			}
			Player->Transform()->SetRelativePos(playerPos);

			Player->RigidBody2D()->SetVelocityX(1);
		}
	}
}

void CWallScript::EndOverlap(CCollider2D* _Other)
{
	if(L"Player" == _Other->GetOwner()->GetName())
	{
		CGameObject* Player = _Other->GetOwner();
		CPlayerScript* PlayerScript = Player->GetScript<CPlayerScript>();

		// 플레이어의 벽 충돌정보를 Reset한다.
		PlayerScript->ResetWallCollision();
	}
}

//Vec3 CWallScript::CalculateOverlapAmount(CCollider2D* _colliderA, CCollider2D* _colliderB)
//{
//	Vec3 posA = _colliderA->GetOwner()->Transform()->GetRelativePos();
//	Vec3 posB = _colliderB->GetOwner()->Transform()->GetRelativePos();
//	Vec3 extentsA = _colliderA->GetOffsetScale() / 2.f;
//	Vec3 extentsB = _colliderB->GetOffsetScale() / 2.f;
//
//	// Calculate the overlapping amounts for each axis
//	float xOverlap = (extentsA.x + extentsB.x) - abs(posA.x - posB.x);
//	float yOverlap = (extentsA.y + extentsB.y) - abs(posA.y - posB.y);
//
//	// Return the overlap amounts
//	return Vec3(xOverlap, yOverlap, 0);
//}

void CWallScript::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_WallType, sizeof(UINT), 1, _File);
	fwrite(&m_bIsEndWall, sizeof(UINT), 1, _File);
}

void CWallScript::LoadFromLevelFile(FILE* _File)
{
	fread(&m_WallType, sizeof(UINT), 1, _File);
	fread(&m_bIsEndWall, sizeof(UINT), 1, _File);
}
