#include "pch.h"
#include "CParticleSystem.h"

#include "CResourceManager.h"
#include "CStructuredBuffer.h"
#include "CTransform.h"
#include "CTimeManager.h"
#include "CAnimator2D.h"

CParticleSystem::CParticleSystem()
	: CRenderComponent(COMPONENT_TYPE::PARTICLESYSTEM)
	, m_ParticleBuffer(nullptr)
	, m_RWBuffer(nullptr)
	, m_ModuleData{}
	, m_AccTime(0.f)
{
	// ��ƼŬ �ִ밳��
	m_ModuleData.iMaxParticleCount = 1000;

	// ��ƼŬ ���� ���, ������
	m_ModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::PARTICLE_SPAWN] = true;
	m_ModuleData.SpawnRate = 20;

	// ��ƼŬ ���� ����
	m_ModuleData.vSpawnColor = Vec3(0.4f, 1.f, 0.4f);

	// ��ƼŬ ���� �ּ�~�ִ� ũ��
	m_ModuleData.vSpawnScaleMin = Vec3(100.f, 100.f, 1.f);
	m_ModuleData.vSpawnScaleMax = Vec3(200.f, 200.f, 1.f);

	// ��ƼŬ ���� ����, Space
	m_ModuleData.SpawnShapeType = 0;	// �ڽ��� ����
	m_ModuleData.vBoxShapeScale = Vec3(500.f, 500.f, 500.f);
	m_ModuleData.Space = 0; // �ùķ��̼� ��ǥ��

	// ��ƼŬ �ּ�~�ִ� ����
	m_ModuleData.MinLifeTime = 3.f;
	m_ModuleData.MaxLifeTime = 5.f;

	// ũ�� ���� ���
	m_ModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::SCALE_CHANGE] = true;
	m_ModuleData.StartScale = 1.5f;
	m_ModuleData.EndScale = 0.2f;

	// ���� ���� ���
	m_ModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::COLOR_CHANGE] = true;
	m_ModuleData.vStartColor = Vec3(0.2f, 0.3f, 1.0f);
	m_ModuleData.vEndColor = Vec3(0.4f, 1.f, 0.4f);

	// �ӵ� �ִ� ���
	m_ModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::ADD_VELOCITY] = true;
	m_ModuleData.AddVelocityType = 0; // 0 : From Center, 1: To Center, 2: Ư�� ����
	m_ModuleData.Speed = 500.f;
	m_ModuleData.vVelocityDir;
	m_ModuleData.OffsetAngle;

	// �巡��(�ӵ� ����) ���
	m_ModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::DRAG] = true;
	m_ModuleData.StartDrag = 500.f;
	m_ModuleData.EndDrag = -500.f;

	// NoiseForce(���� ��) ���
	m_ModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::NOISE_FORCE] = false;
	m_ModuleData.fNoiseTerm = 0.3f;
	m_ModuleData.fNoiseForce = 50.f;

	// ���� ���
	m_ModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::RENDER] = true;
	m_ModuleData.VelocityAlignment = true;	// �ӵ� ����
	m_ModuleData.VelocityScale = true;		// �ӵ� ��� ũ��
	m_ModuleData.vMaxVelocityScale = Vec3(20.f, 1.f, 1.f);
	m_ModuleData.vMaxSpeed = 500.f;

	
	// ��ƼŬ�� ����Ʈ �޽� ����
	SetMesh(CResourceManager::GetInst()->FindRes<CMesh>(L"PointMesh"));

	// ��ƼŬ�� ���� ����
	SetMaterial(CResourceManager::GetInst()->FindRes<CMaterial>(L"ParticleRenderMtrl"));

	// ��ƼŬ ������Ʈ ��ǻƮ ���̴�	
	m_UpdateCS = (CParticleUpdateShader*)CResourceManager::GetInst()->FindRes<CComputeShader>(L"ParticleUpdateCS").Get();

	//Ptr<CTexture> pParticleTex = CResourceManager::GetInst()->Load<CTexture>(L"Particle_0", L"texture\\particle\\AlphaCircle.png");
	//SetParticleTex(pParticleTex);

	// tParticle ����ü X �ִ���ƼŬ ���� ��ŭ�� ����ȭ���� ����, �ʱⰪ�� �־��ش�.
	m_ParticleBuffer = new CStructuredBuffer;
	m_ParticleBuffer->Create(sizeof(tParticle), m_ModuleData.iMaxParticleCount, SB_TYPE::READ_WRITE, false);

	// SpawnCount�� �����ϴ� �뵵�� RW ���� ����(�ǽð����� �����ǹǷ�, RW�ɼ�)
	m_RWBuffer = new CStructuredBuffer;
	m_RWBuffer->Create(sizeof(tRWParticleBuffer), 1, SB_TYPE::READ_WRITE, true);

	// �� ��ƼŬ���� ����� ��� ������ �����ϴ� �뵵�� ����ȭ���� ����(�������� �����Ƿ� ReadOnly)
	m_ModuleDataBuffer = new CStructuredBuffer;
	m_ModuleDataBuffer->Create(sizeof(tParticleModule), 1, SB_TYPE::READ_ONLY, true);
}

CParticleSystem::~CParticleSystem()
{
	if (nullptr != m_ParticleBuffer)
		delete m_ParticleBuffer;

	if (nullptr != m_RWBuffer)
		delete m_RWBuffer;

	if (nullptr != m_ModuleDataBuffer)
		delete m_ModuleDataBuffer;
}

void CParticleSystem::finaltick()
{
	// ���� ����Ʈ ���
	// 1�� ���� �ð�
	float fTimePerCount = 1.f / (float)m_ModuleData.SpawnRate;
	m_AccTime += DT;

	// �����ð��� ���� �����ð��� �Ѿ��
	if (fTimePerCount < m_AccTime)
	{
		// �������� ���ڱ� �������ų� �ؼ� DT�� �ѹ��� ū ������ ���� ���,
		// �ʰ��� �ð��� ��������Ʈ�� ����� �� ��ŭ�� ��ƼŬ�� �ѹ��� ������,
		// ��������ŭ�� �ð����� �ʱ�ȭ�Ǿ���Ѵ�.
 
		// �ʰ� ���� ==> ���� ����
		float fData = m_AccTime / fTimePerCount;

		// �������� ���� �ð�
		m_AccTime = fTimePerCount * (fData - floor(fData));

		// ������ ���� �̹� �����ӿ� �����ؾ��� ��ƼŬ ������ rw���ۿ� ���ε��Ѵ�.
		tRWParticleBuffer rwbuffer = { (int)fData, };
		m_RWBuffer->SetData(&rwbuffer);
	}

	// ��ⵥ���� ���� ���ۿ� ��ⵥ���� ���ε�
	m_ModuleDataBuffer->SetData(&m_ModuleData);

	// ��ƼŬ ������Ʈ CS�� ��ƼŬ����,  ���ε�
	m_UpdateCS->SetParticleBuffer(m_ParticleBuffer);
	m_UpdateCS->SetRWParticleBuffer(m_RWBuffer);
	m_UpdateCS->SetModuleData(m_ModuleDataBuffer);

	// �������ؽ�ó ���ε�
	m_UpdateCS->SetNoiseTexture(CResourceManager::GetInst()->FindRes<CTexture>(L"texture\\Noise\\noise_01.png"));

	// �θ�(������Ʈ)�� ��ġ ����
	m_UpdateCS->SetParticleObjectPos(Transform()->GetWorldPos());

	// ��ƼŬ ���ڰ��� ������� CS ����
	m_UpdateCS->Execute();
}

void CParticleSystem::render()
{
	// ��ƼŬ �ý����� ������� ������ ���� Transform ������Ʈ
	Transform()->UpdateData();

	// ��ƼŬ ���۸� t20���� ���ε�(�������̹Ƿ� CS�� �ƴ� ���������� ������ ���ε�)
	m_ParticleBuffer->UpdateData(20, PIPELINE_STAGE::PS_ALL);

	// ��� ������ t21 �� ���ε�(��ƼŬ ScaleFactor�� ���� ���� ������ ���)
	m_ModuleDataBuffer->UpdateData(21, PIPELINE_STAGE::PS_GEOMETRY);

	// ��ƼŬ �ؽ�ó �ε�, ���ε�
	//Ptr<CTexture> pParticleTex = CResourceManager::GetInst()->Load<CTexture>(L"texture\\particle\\AlphaCircle.png", L"texture\\particle\\AlphaCircle.png");
	
	if (m_ParticleTex != nullptr)
	{
		GetMaterial()->SetTexParam(TEX_0, m_ParticleTex);
	}
	else
	{
		GetMaterial()->SetTexParam(TEX_0, nullptr);
	}

	// Animator2D ������Ʈ�� �ִٸ�
	if (Animator2D())
	{
		Animator2D()->UpdateData();
	}

	GetMaterial()->UpdateData();
	GetMesh()->render_particle(m_ModuleData.iMaxParticleCount);
	
	// ��ƼŬ ���� ���ε� ����
	m_ParticleBuffer->Clear();
	m_ModuleDataBuffer->Clear();
}

void CParticleSystem::SaveToLevelFile(FILE* _File)
{
	// �θ� RenderComponent Save (Mesh, Mtrl)
	CRenderComponent::SaveToLevelFile(_File);

	// ��ƼŬ ��� ������ ����ü ��°�� ����
	fwrite(&m_ModuleData, sizeof(tParticleModule), 1, _File);

	// ��ƼŬ ������Ʈ �� ComputeShader ���� ����
	SaveResourceRef(m_UpdateCS.Get(), _File);
}

void CParticleSystem::LoadFromLevelFile(FILE* _File)
{
	// �θ� RenderComponent Load (Mesh, Mtrl)
	CRenderComponent::LoadFromLevelFile(_File);

	// ��ƼŬ ��� ������ ����ü �ҷ�����
	fread(&m_ModuleData, sizeof(tParticleModule), 1, _File);

	// ��ƼŬ ������Ʈ ���̴� ���� ����
	int i = 0;
	fread(&i, sizeof(i), 1, _File);

	if (i)
	{
		// ������ CS�� ���� ��� �ش� ���̴��� Ű��, ��ΰ� �ҷ�����
		wstring strKey, strRelativePath;
		LoadWString(strKey, _File);
		LoadWString(strRelativePath, _File);

		// CS�� ��� �������ҽ��̹Ƿ� ��� ��ΰ� �ǹ̰� ����. Ű�����θ� �ҷ��� �� ����
		m_UpdateCS = (CParticleUpdateShader*)CResourceManager::GetInst()->FindRes<CComputeShader>(strKey).Get();
	}
}
