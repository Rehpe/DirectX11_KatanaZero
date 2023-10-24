#include "pch.h"
#include "CRigidBody2D.h"

#include "CTimeManager.h"
#include "CTransform.h"
#include "CLevelManager.h"
#include "CLevel.h"

CRigidBody2D::CRigidBody2D()
	: CComponent(COMPONENT_TYPE::RIGIDBODY2D)
	, m_fMass(1.f)
	, m_bGround(false)
	, m_vMaxVelocity(Vec3(400.f, 500.f, 0.f))
	, m_fGravityAccel(1000.f)
	, m_fFriction(15.f)
	, m_fGravityScale(1.f)
	, m_bIsGravity(true)
{
}
	

CRigidBody2D::CRigidBody2D(const CRigidBody2D& _Other)
	: CComponent(COMPONENT_TYPE::RIGIDBODY2D)
	, m_fMass(_Other.m_fMass)
	, m_vVelocity(_Other.m_vVelocity)
	, m_vMaxVelocity(_Other.m_vMaxVelocity)
	, m_fGravityAccel(_Other.m_fGravityAccel)
	, m_fFriction(_Other.m_fFriction)
	, m_vForce(_Other.m_vForce)
	, m_fGravityScale(_Other.m_fGravityScale)
	, m_bGround(false)
	, m_bIsGravity(true)
{
}

CRigidBody2D::~CRigidBody2D()
{
}

void CRigidBody2D::finaltick()
{
	CLevel* CurLevel = CLevelManager::GetInst()->GetCurLevel();
	if (CurLevel->GetState() == LEVEL_STATE::STOP
		|| CurLevel->GetState() == LEVEL_STATE::PAUSE)
		return;
	
	//���ӵ� = �� / ����
	Vec3 vAccel = m_vForce / m_fMass;

	//���ӵ��� �̿��ؼ� �ӵ��� ������Ų��.
	m_vVelocity += vAccel * DT;

	if((!m_bGround && !m_bOnSlope) && m_bIsGravity)
	{
		//���߿� �ִ� ��� -> �߷� ����
		Vec3 vGravityAccel = Vec3(0.f, m_fGravityAccel * m_fGravityScale, 0.f);
		m_vVelocity -= vGravityAccel * DT;
	}

	//�ִ�ӵ� ����
	if (fabsf(m_vMaxVelocity.x) < fabsf(m_vVelocity.x))
	{
		//������ ������ä(1 �Ǵ� -1) ���Ѽӵ��� ���ؼ� ���Ѽӵ���ŭ ������ �ɾ��ش�
		m_vVelocity.x = (m_vVelocity.x / fabsf(m_vVelocity.x)) * fabsf(m_vMaxVelocity.x);
	}
	if ((m_vMaxVelocity.y) <(m_vVelocity.y))
	{
		//������ ������ä(1 �Ǵ� -1) ���Ѽӵ��� ���ؼ� ���Ѽӵ���ŭ ������ �ɾ��ش�
		m_vVelocity.y = (m_vVelocity.y / fabsf(m_vVelocity.y)) * fabsf(m_vMaxVelocity.y);
	}

	// Friction
	CalcFriction();

	//������Ʈ�� ��ġ�� �˾Ƴ���.
	Vec3 vPos = Transform()->GetRelativePos();

	//���� �ӷ¸�ŭ ������Ʈ�� �̵���Ų��.
	vPos.x += m_vVelocity.x * DT;
	vPos.y += m_vVelocity.y * DT;

	Transform()->SetRelativePos(vPos);

	//��ġ �ݿ��� ������ ���� 0���� �ʱ�ȭ�Ѵ�.
	m_vForce = Vec3(0.f, 0.f, 0.f);
}

void CRigidBody2D::SetGround(bool _b)
{
	m_bGround = _b;
	if (m_bGround)
	{
		m_vVelocity.y = 0;
	}
}

void CRigidBody2D::SetOnSlope(bool _b)
{
	m_bOnSlope = _b;
	if (m_bOnSlope)
	{
		
	}
}

void CRigidBody2D::CalcFriction()
{
	if ((!m_bGround && !m_bOnSlope) && !m_bMovingInput)
	{
		float frictionForce = -m_vVelocity.x * m_fFriction * 0.07f;
		m_vVelocity.x += frictionForce * DT;

		// Prevent jittering when the velocity is near zero
		if (abs(m_vVelocity.x) < 0.01f)
		{
			m_vVelocity.x = 0.f;
		}
	}
	else if (m_bGround && !m_bMovingInput) // On flat ground and not moving
	{
		
		float frictionForce = -m_vVelocity.x * m_fFriction;
		m_vVelocity.x += frictionForce * DT;

		// Prevent jittering when the velocity is near zero
		if (abs(m_vVelocity.x) < 0.01f)
		{
			m_vVelocity.x = 0.f;
		}
	}
	else if (m_bOnSlope && !m_bMovingInput) // On ramp and not moving
	{
		//Vec3 frictionDirection = Vec3(-cos(m_fSlopeAngle), -sin(m_fSlopeAngle), 0);

		//float frictionForce = m_vVelocity.Dot(frictionDirection) * m_fFriction;

		float frictionForce = m_fGravityAccel * sin(m_fSlopeAngle);
		frictionForce* m_fFriction;
		
		m_vVelocity.x += frictionForce * DT;

		// Apply friction force
		//m_vVelocity += frictionDirection * frictionForce * DT;

		// Prevent jittering when the velocity is near zero
		if (abs(m_vVelocity.x) < 0.01f && abs(m_vVelocity.y) < 0.01f)
		{
			m_vVelocity.x = 0.f;
			m_vVelocity.y = 0.f;
		}
	}
}


void CRigidBody2D::SaveToLevelFile(FILE* _File)
{
	return;
}

void CRigidBody2D::LoadFromLevelFile(FILE* _File)
{
	return;
}
