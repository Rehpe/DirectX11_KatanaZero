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
	// 파티클 최대개수
	m_ModuleData.iMaxParticleCount = 1000;

	// 파티클 스폰 모듈, 스폰율
	m_ModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::PARTICLE_SPAWN] = true;
	m_ModuleData.SpawnRate = 20;

	// 파티클 스폰 색상
	m_ModuleData.vSpawnColor = Vec3(0.4f, 1.f, 0.4f);

	// 파티클 스폰 최소~최대 크기
	m_ModuleData.vSpawnScaleMin = Vec3(100.f, 100.f, 1.f);
	m_ModuleData.vSpawnScaleMax = Vec3(200.f, 200.f, 1.f);

	// 파티클 스폰 영역, Space
	m_ModuleData.SpawnShapeType = 0;	// 박스형 스폰
	m_ModuleData.vBoxShapeScale = Vec3(500.f, 500.f, 500.f);
	m_ModuleData.Space = 0; // 시뮬레이션 좌표계

	// 파티클 최소~최대 수명
	m_ModuleData.MinLifeTime = 3.f;
	m_ModuleData.MaxLifeTime = 5.f;

	// 크기 변경 모듈
	m_ModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::SCALE_CHANGE] = true;
	m_ModuleData.StartScale = 1.5f;
	m_ModuleData.EndScale = 0.2f;

	// 색상 변경 모듈
	m_ModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::COLOR_CHANGE] = true;
	m_ModuleData.vStartColor = Vec3(0.2f, 0.3f, 1.0f);
	m_ModuleData.vEndColor = Vec3(0.4f, 1.f, 0.4f);

	// 속도 주는 모듈
	m_ModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::ADD_VELOCITY] = true;
	m_ModuleData.AddVelocityType = 0; // 0 : From Center, 1: To Center, 2: 특정 각도
	m_ModuleData.Speed = 500.f;
	m_ModuleData.vVelocityDir;
	m_ModuleData.OffsetAngle;

	// 드래그(속도 제한) 모듈
	m_ModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::DRAG] = true;
	m_ModuleData.StartDrag = 500.f;
	m_ModuleData.EndDrag = -500.f;

	// NoiseForce(랜덤 힘) 모듈
	m_ModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::NOISE_FORCE] = false;
	m_ModuleData.fNoiseTerm = 0.3f;
	m_ModuleData.fNoiseForce = 50.f;

	// 렌더 모듈
	m_ModuleData.ModuleCheck[(UINT)PARTICLE_MODULE::RENDER] = true;
	m_ModuleData.VelocityAlignment = true;	// 속도 정렬
	m_ModuleData.VelocityScale = true;		// 속도 비례 크기
	m_ModuleData.vMaxVelocityScale = Vec3(20.f, 1.f, 1.f);
	m_ModuleData.vMaxSpeed = 500.f;

	
	// 파티클용 포인트 메쉬 세팅
	SetMesh(CResourceManager::GetInst()->FindRes<CMesh>(L"PointMesh"));

	// 파티클용 재질 세팅
	SetMaterial(CResourceManager::GetInst()->FindRes<CMaterial>(L"ParticleRenderMtrl"));

	// 파티클 업데이트 컴퓨트 쉐이더	
	m_UpdateCS = (CParticleUpdateShader*)CResourceManager::GetInst()->FindRes<CComputeShader>(L"ParticleUpdateCS").Get();

	//Ptr<CTexture> pParticleTex = CResourceManager::GetInst()->Load<CTexture>(L"Particle_0", L"texture\\particle\\AlphaCircle.png");
	//SetParticleTex(pParticleTex);

	// tParticle 구조체 X 최대파티클 개수 만큼의 구조화버퍼 생성, 초기값을 넣어준다.
	m_ParticleBuffer = new CStructuredBuffer;
	m_ParticleBuffer->Create(sizeof(tParticle), m_ModuleData.iMaxParticleCount, SB_TYPE::READ_WRITE, false);

	// SpawnCount를 전달하는 용도의 RW 버퍼 생성(실시간으로 수정되므로, RW옵션)
	m_RWBuffer = new CStructuredBuffer;
	m_RWBuffer->Create(sizeof(tRWParticleBuffer), 1, SB_TYPE::READ_WRITE, true);

	// 각 파티클들이 사용할 모듈 변수를 전달하는 용도의 구조화버퍼 생성(수정되지 않으므로 ReadOnly)
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
	// 스폰 레이트 계산
	// 1개 스폰 시간
	float fTimePerCount = 1.f / (float)m_ModuleData.SpawnRate;
	m_AccTime += DT;

	// 누적시간이 개당 생성시간을 넘어서면
	if (fTimePerCount < m_AccTime)
	{
		// 프레임이 갑자기 떨어지거나 해서 DT가 한번에 큰 단위로 들어올 경우,
		// 초과된 시간을 스폰레이트로 나누어서 그 몫만큼의 파티클을 한번에 만들어내고,
		// 나머지만큼의 시간으로 초기화되어야한다.
 
		// 초과 배율 ==> 생성 개수
		float fData = m_AccTime / fTimePerCount;

		// 나머지는 남은 시간
		m_AccTime = fTimePerCount * (fData - floor(fData));

		// 위에서 구한 이번 프레임에 생성해야할 파티클 개수를 rw버퍼에 바인딩한다.
		tRWParticleBuffer rwbuffer = { (int)fData, };
		m_RWBuffer->SetData(&rwbuffer);
	}

	// 모듈데이터 전용 버퍼에 모듈데이터 바인딩
	m_ModuleDataBuffer->SetData(&m_ModuleData);

	// 파티클 업데이트 CS에 파티클버퍼,  바인딩
	m_UpdateCS->SetParticleBuffer(m_ParticleBuffer);
	m_UpdateCS->SetRWParticleBuffer(m_RWBuffer);
	m_UpdateCS->SetModuleData(m_ModuleDataBuffer);

	// 노이즈텍스처 바인딩
	m_UpdateCS->SetNoiseTexture(CResourceManager::GetInst()->FindRes<CTexture>(L"texture\\Noise\\noise_01.png"));

	// 부모(오브젝트)의 위치 지정
	m_UpdateCS->SetParticleObjectPos(Transform()->GetWorldPos());

	// 파티클 입자값을 계산해줄 CS 실행
	m_UpdateCS->Execute();
}

void CParticleSystem::render()
{
	// 파티클 시스템의 행렬정보 전달을 위해 Transform 업데이트
	Transform()->UpdateData();

	// 파티클 버퍼를 t20번에 바인딩(렌더링이므로 CS가 아닌 파이프라인 과정에 바인딩)
	m_ParticleBuffer->UpdateData(20, PIPELINE_STAGE::PS_ALL);

	// 모듈 데이터 t21 에 바인딩(파티클 ScaleFactor에 따른 입자 생성에 사용)
	m_ModuleDataBuffer->UpdateData(21, PIPELINE_STAGE::PS_GEOMETRY);

	// 파티클 텍스처 로드, 바인딩
	//Ptr<CTexture> pParticleTex = CResourceManager::GetInst()->Load<CTexture>(L"texture\\particle\\AlphaCircle.png", L"texture\\particle\\AlphaCircle.png");
	
	if (m_ParticleTex != nullptr)
	{
		GetMaterial()->SetTexParam(TEX_0, m_ParticleTex);
	}
	else
	{
		GetMaterial()->SetTexParam(TEX_0, nullptr);
	}

	// Animator2D 컴포넌트가 있다면
	if (Animator2D())
	{
		Animator2D()->UpdateData();
	}

	GetMaterial()->UpdateData();
	GetMesh()->render_particle(m_ModuleData.iMaxParticleCount);
	
	// 파티클 버퍼 바인딩 해제
	m_ParticleBuffer->Clear();
	m_ModuleDataBuffer->Clear();
}

void CParticleSystem::SaveToLevelFile(FILE* _File)
{
	// 부모 RenderComponent Save (Mesh, Mtrl)
	CRenderComponent::SaveToLevelFile(_File);

	// 파티클 모듈 데이터 구조체 통째로 저장
	fwrite(&m_ModuleData, sizeof(tParticleModule), 1, _File);

	// 파티클 업데이트 용 ComputeShader 참조 저장
	SaveResourceRef(m_UpdateCS.Get(), _File);
}

void CParticleSystem::LoadFromLevelFile(FILE* _File)
{
	// 부모 RenderComponent Load (Mesh, Mtrl)
	CRenderComponent::LoadFromLevelFile(_File);

	// 파티클 모듈 데이터 구조체 불러오기
	fread(&m_ModuleData, sizeof(tParticleModule), 1, _File);

	// 파티클 업데이트 셰이더 참조 여부
	int i = 0;
	fread(&i, sizeof(i), 1, _File);

	if (i)
	{
		// 참조한 CS가 있을 경우 해당 셰이더의 키값, 경로값 불러오기
		wstring strKey, strRelativePath;
		LoadWString(strKey, _File);
		LoadWString(strRelativePath, _File);

		// CS의 경우 엔진리소스이므로 사실 경로가 의미가 없다. 키값으로만 불러온 후 세팅
		m_UpdateCS = (CParticleUpdateShader*)CResourceManager::GetInst()->FindRes<CComputeShader>(strKey).Get();
	}
}
