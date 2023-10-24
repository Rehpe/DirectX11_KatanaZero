#include "pch.h"
#include "CTransform.h"

#include "CDevice.h" 
#include "CConstBuffer.h"

CTransform::CTransform()
	: CComponent(COMPONENT_TYPE::TRANSFORM)
	, m_vRelativeScale(Vec3(1.f, 1.f, 1.f))			// �⺻ �ʱⰪ�� 0,0,0���� 0������ ���� �ȵǹǷ� �ʱ� �������� 1�� ����
	, m_bAbsolute(false)
	, m_vRelativeDir{
		  Vec3(1.f, 0.f, 0.f)		//������
		, Vec3(0.f, 1.f, 0.f)		//��
		, Vec3(0.f, 0.f, 1.f)}		//����
{
}

CTransform::~CTransform()
{
}

void CTransform::finaltick()
{
	// ũ�� ���
	m_matWorldScale = XMMatrixIdentity();		// �������(�밢���� 1, �������� 0�� 4*4 ���. ������ �� �ڱ� �ڽ��� ���´�)
	m_matWorldScale = XMMatrixScaling(m_vRelativeScale.x, m_vRelativeScale.y, m_vRelativeScale.z);	// �� x,y,z ũ�Ⱚ�� 11, 22, 33�� �־���

	// ȸ�� ���
	Matrix matRot = XMMatrixIdentity();			
	matRot *= XMMatrixRotationX(m_vRelativeRot.x);		// �� �ະ�� ȸ������� ������ִ� �Լ�
	matRot *= XMMatrixRotationY(m_vRelativeRot.y);
	matRot *= XMMatrixRotationZ(m_vRelativeRot.z);

	// �̵� ���. ũ��� ȸ�� ����� �� �� �� �������� �����־���Ѵ�.
	Matrix matTranslation = XMMatrixTranslation(m_vRelativePos.x, m_vRelativePos.y, m_vRelativePos.z);

	// ���� ���� ��� = ũ�� x ȸ�� X �̵�
	m_matWorld = m_matWorldScale * matRot * matTranslation;

	// ===== ���ϰ��� �ϴ� ���� ��ǥ����̶�� ������ǥ�� 1��, ���⺤���� ��쿡�� ������ǥ�� 0���� �����Ѵ� ====

	// ���⺤�͸� ���� ������ float4(Vec4)�� Ȯ���� �� 4��° ��ǥ(������ǥ)�� 1�� �ƴ� 0���� Ȯ���Ѵ�
	// World ��Ŀ��� ��Ȯ�� ȸ�� ��ĸ��� ������ �� ���� ��� World��� ��ü�� ���� ������ ���� ���� �ִµ�, 
	// ���� ��ǥ�� 1�� Ȯ���� ��� World����� 4�� 1,2,3���� �̵������� ����Ǿ� ���⺤�Ͱ� Ʋ������ �����̴�
	// ���� �̵� ����� ������ ���� �ʰ� �ϱ� ���� ������ǥ�� 0���� ����
	
	// �׷��� ������ǥ�� 0���� �ξ World��� �� ũ�� ��Ŀ��� ������ ���� ���ۿ� ���� ������,
	// ��Ȯ�� ȸ������� ���ϴ� ���� �ƴ�, World����� ���Ͽ� ���⺤�͸� ���� ������ 
	// �ݵ�� ��� ��� �Ϸ� �� Normalize(����ȭ)�� ���̸� 1�� ����� �۾��� �� ���־���Ѵ�.

	// ���� ���(�⺻ �� ���) �� ȸ�� ����� ���Ͽ� ������Ʈ�� �� �� ������ ���Ѵ�
	Vec3 vDefaultDir[3] = {
		  Vec3(1.f, 0.f, 0.f)		//������(R)
		, Vec3(0.f, 1.f, 0.f)		//��(UP)
		, Vec3(0.f, 0.f, 1.f) };	//����(Front)

	for (int i = 0; i < 3; i++)
	{
		// Coord/Normal �� �Լ��� ���� : Coord�� ��ǥ�� ���ϴ� �Լ�, ���� ������ǥ�� 1�� ���� ���ϴ� �Լ�
		// �츮�� ������ ���ؾ��ϹǷ� �̶����� Normal, �� ������ǥ�� 0���� Ȯ���ϴ� �Լ��� ����ؾ��Ѵ�.

		// ���� matRotó�� ��Ȯ�� ȸ������� ���� �� ���� World��ķ� ������ ���� ���, �ݵ�� Normalize�Ѵ�.
		//m_vRelativeDir[i] = XMVector3TransformNormal(vDefalutDir[i], m_matWorld);
		//m_vRelativeDir[i].Normalize();
		
		// �Ʒ�ó�� ȸ����ĸ� ���� �� �ִٸ�, ������ ȸ������� 3x3 �ȿ� ��ȿ���� �־� �׹�° �ڸ��� ������ ���� �����Ƿ� 
		// Coord, Normal ��� ������ ���ص� ����� ����.
		//XMVector3TransformCoord(vDefalutDir[i], matRot);

		// �⺻������ ���� ������ ������ ������(���ý����̽����� ����)���� �����صд�. �θ� �ִٸ� �ؿ��� �ٲ� ��
		m_vWorldDir[i] = m_vRelativeDir[i] = XMVector3TransformNormal(vDefaultDir[i], matRot);
	}
	
	// Object Hierachy (������Ʈ ��������) : �θ� �ִ��� Ȯ���Ѵ�.
	CGameObject* pParent = GetOwner()->GetParent();
	if (pParent)
	{
		// �θ� �ִ� ���, �� �ڽ� ��ü�� ���'

		if (m_bAbsolute)
		{
			// �ۼַ�Ʈ �ɼ�, �� ���밪 �ɼ��� �����ִ� ���

			// �θ��� ������İ� ������� �� ũ����ĸ� ���� ��� �����´�.
			// tick-finaltick�� �θ�-�ڽ� ������ ����ǹǷ�, ������ ��������� ���� ������Ʈ�� �θ� ��������� �̹� ������ �����̴�
			Matrix matParentWorld = pParent->Transform()->m_matWorld;
			Matrix matParentScale = pParent->Transform()->m_matWorldScale;

			// �ۼַ�Ʈ(���밪) �ɼ��� �����ٴ� ����, �θ��� ũ��/ȸ��/�̵� �� ũ�⿡�� �����ް� ���� �ʴٴ� ���̹Ƿ�
			// �θ��� ũ�� ����� ������� ���� �����־� ũ�� �κ��� ������ķ� �����
			// �ڽ� ������Ʈ�� ������Ŀ� �θ��� ũ�� ����� ��ġ�� ������ ���� ������ش�.

			// �θ��� ũ�� ����� ������� ����� �Լ� MatrixInverse
			Matrix matParentScaleInv = XMMatrixInverse(nullptr, matParentScale);

			// �ۼַ�Ʈ ������Ʈ�� ���� ���� ��� = ���� ������� * �θ�ũ����� �� * �θ� ����(ũ��/ȸ��/�̵�)
			m_matWorld = m_matWorld * matParentScaleInv * matParentWorld;
		}
		else
		{
			// �ڽ��� ���� ���� ��� : ������ Relative ���(�������) X �θ��� �������
			m_matWorld *= pParent->Transform()->GetWorldMat();
			m_matWorldScale *= pParent->Transform()->GetWorldScaleMat();
		}

		// �θ� �ִ� ��� �ڽ��� WorldDir(���� ����)�� ������Ʈ�� ������ ������ �θ��� ��������� ���� �� ��ֶ����� �� ���̴�
		// �̹� ������ ���� ȸ�� * �θ��� ȸ���� ��� ����� ���� ���� ����� �������Ƿ�,
		// ���� ȸ������ ������� ���� ������Ŀ� ���� ��������� ���� �� ũ�� ������ ���ֱ� ���� Normalize �ϴ� ������ ���Ѵ�.
		for (int i = 0; i < 3; ++i)
		{
			m_vWorldDir[i] = XMVector3TransformNormal(vDefaultDir[i], m_matWorld);
			m_vWorldDir[i].Normalize();
		}
	}
}

void CTransform::UpdateData()
{
	// ��ġ���� ������ۿ� ���� �� ���ε�

	// Device���� Transform �뵵�� ������� ������
	CConstBuffer* pTransformBuffer = CDevice::GetInst()->GetConstBuffer(CB_TYPE::TRANSFORM);

	// View�� Proj ����� Camera�� finaltick���� ���� ä���� ��
	g_transform.matWorld = m_matWorld;
	g_transform.matWV = g_transform.matWorld * g_transform.matView;
	g_transform.matWVP = g_transform.matWV * g_transform.matProj;

	// ��ġ�� Map/Unmap
	pTransformBuffer->SetData(&g_transform);
	
 	// ��� ���� �������Ϳ� ���ε�
	pTransformBuffer->UpdateData();
}

void CTransform::SaveToLevelFile(FILE* _File)
{
	// Dir, Matrix�� ���� ��� �� finaltick���� �����ϴ� ���̹Ƿ� ������ �ʿ� ����
	fwrite(&m_vRelativePos, sizeof(Vec3), 1, _File);
	fwrite(&m_vRelativeScale, sizeof(Vec3), 1, _File);
	fwrite(&m_vRelativeRot, sizeof(Vec3), 1, _File);
	fwrite(&m_bAbsolute, sizeof(bool), 1, _File);
}

void CTransform::LoadFromLevelFile(FILE* _File)
{
	fread(&m_vRelativePos, sizeof(Vec3), 1, _File);
	fread(&m_vRelativeScale, sizeof(Vec3), 1, _File);
	fread(&m_vRelativeRot, sizeof(Vec3), 1, _File);
	fread(&m_bAbsolute, sizeof(bool), 1, _File);
}

