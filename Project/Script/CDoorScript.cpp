#include "pch.h"
#include "CDoorScript.h"

#include <Engine\CAnim2D.h>
#include <Engine\CLevel.h>
#include <Engine\CLayer.h>

#include "CGameManagerScript.h"
#include "CEnemyScript.h"

CDoorScript::CDoorScript()
	: CScript((UINT)SCRIPT_TYPE::DOORSCRIPT)
	, m_eDoorState((UINT)DOOR_STATE::CLOSE)
{
	AddScriptParam(SCRIPT_PARAM::INT, &m_iDoorIdx, "Door Index");
}

CDoorScript::~CDoorScript()
{
}


void CDoorScript::begin()
{
	m_eDoorState = (UINT)DOOR_STATE::CLOSE;

	if (GetOwner()->Animator2D() == nullptr)
		return;

	GetOwner()->Animator2D()->Play(L"DoorClosed", true);
}

void CDoorScript::tick()
{
	if ((UINT)DOOR_STATE::CLOSE == m_eDoorState)
	{
	}
	else
	{
		// 문이 열린 경우
		if (GetOwner()->Collider2D() == nullptr)
			return;

		// Door Open -> 충돌체 끄기
		GetOwner()->Collider2D()->SetColliderActivate(false);
		
		// Door 번호에 해당하는 Enemy들 Notice 상태로 전환
		CLevel* Level = CLevelManager::GetInst()->GetCurLevel();
		vector<CGameObject*> Enemys = Level->FindLayerByName(L"Monster")->GetParentObject();
		for (size_t i = 0; i < Enemys.size(); i++)
		{
			CEnemyScript* EnemyScript = Enemys[i]->GetScript<CEnemyScript>();

			// 열린 Door Idx 를 NoticedDoor로 가지고 있는 Enemy에게
			if ((!EnemyScript->IsDead()) && (!EnemyScript->IsNoticed()) && (m_iDoorIdx == EnemyScript->GetNoticedDoorIdx()))
			{
				Enemys[i]->FSM()->ChangeState(L"Run");
			}
		}

		if (GetOwner()->Animator2D()->GetCurAnim()->GetName() == L"DoorOpened")
			return;
		
		GetOwner()->Animator2D()->Play(L"DoorOpened", false);

	}
}

void CDoorScript::BeginOverlap(CCollider2D* _Other)
{
	if ((UINT)DOOR_STATE::CLOSE == m_eDoorState)
	{
		if (_Other->RigidBody2D() == nullptr)
			return;

		_Other->GetOwner()->RigidBody2D()->SetVelocityX(0);
	}
}

void CDoorScript::OnOverlap(CCollider2D* _Other)
{
	if ((UINT)DOOR_STATE::CLOSE == m_eDoorState)
	{
		CollisionDir collDir = GetCollisionDirX(_Other, GetOwner()->Collider2D());

		// 부딪친 위치에 따라 AntiOverlap
		if (collDir == CollisionDir::RIGHT)
		{
			// 오브젝트가 문의 오른쪽에서 충돌할 경우
			Vec3 overlap = CalculateOverlapAmount(_Other, GetOwner()->Collider2D());
			float threshhold = 10.f;

			Vec3 ObjectPos = _Other->GetOwner()->Transform()->GetRelativePos();
			if (overlap.x >= threshhold)
			{
				ObjectPos.x += overlap.x;
				_Other->GetOwner()->Transform()->SetRelativePos(ObjectPos);
			}
		}
		else if (collDir == CollisionDir::LEFT)
		{
			// 오브젝트가 문의 왼쪽에서 충돌할 경우
			Vec3 overlap = CalculateOverlapAmount(_Other, GetOwner()->Collider2D());
			float threshhold = 10.f;

			Vec3 ObjectPos = _Other->GetOwner()->Transform()->GetRelativePos();
			if (overlap.x >= threshhold)
			{
				ObjectPos.x -= overlap.x;
				_Other->GetOwner()->Transform()->SetRelativePos(ObjectPos);
			}
		}
	}
}

void CDoorScript::EndOverlap(CCollider2D* _Other)
{
}

void CDoorScript::SaveToLevelFile(FILE* _File)
{
	fwrite(&m_iDoorIdx, sizeof(Vec3), 1, _File);
}

void CDoorScript::LoadFromLevelFile(FILE* _File)
{
	fread(&m_iDoorIdx, sizeof(Vec3), 1, _File);
}
