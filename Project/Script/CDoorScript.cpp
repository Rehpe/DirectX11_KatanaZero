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
		// ���� ���� ���
		if (GetOwner()->Collider2D() == nullptr)
			return;

		// Door Open -> �浹ü ����
		GetOwner()->Collider2D()->SetColliderActivate(false);
		
		// Door ��ȣ�� �ش��ϴ� Enemy�� Notice ���·� ��ȯ
		CLevel* Level = CLevelManager::GetInst()->GetCurLevel();
		vector<CGameObject*> Enemys = Level->FindLayerByName(L"Monster")->GetParentObject();
		for (size_t i = 0; i < Enemys.size(); i++)
		{
			CEnemyScript* EnemyScript = Enemys[i]->GetScript<CEnemyScript>();

			// ���� Door Idx �� NoticedDoor�� ������ �ִ� Enemy����
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

		// �ε�ģ ��ġ�� ���� AntiOverlap
		if (collDir == CollisionDir::RIGHT)
		{
			// ������Ʈ�� ���� �����ʿ��� �浹�� ���
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
			// ������Ʈ�� ���� ���ʿ��� �浹�� ���
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
