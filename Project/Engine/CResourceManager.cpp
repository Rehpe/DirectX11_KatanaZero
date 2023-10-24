#include "pch.h"
#include "CResourceManager.h"

#include "CPrefab.h"
#include "CSetColorShader.h"

#include "CParticleUpdateShader.h"


CResourceManager::CResourceManager()
	: m_Changed(false)
{
}

CResourceManager::~CResourceManager()
{
}

void CResourceManager::init()
{
	InitSound();

	CreateDefaultMesh();
	CreateDefaultGraphicsShader();
	CreateDefaultComputeShader();
	CreateDefaultMaterial();

	LoadDefaultTexture();
}

void CResourceManager::tick()
{
	// 변경사항 여부를 false로 초기화
	m_Changed = false;
}

void CResourceManager::InitSound()
{
	FMOD::System_Create(&CSound::g_pFMOD);

	if (nullptr == CSound::g_pFMOD)
	{
		assert(nullptr);
	}

	// 32개 채널 생성
	CSound::g_pFMOD->init(32, FMOD_DEFAULT, nullptr);
}

void CResourceManager::CreateDefaultMesh()
{
	vector<Vtx> vecVtx;
	vector<UINT> vecIdx;
	Vtx v;

	Ptr<CMesh> pMesh = nullptr;
	
	// ========================
	// PointMesh 생성(파티클용)
	// ========================
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);

	UINT idx = 0;

	pMesh = new CMesh(true);
	pMesh->Create(&v, 1, &idx, 1);
	AddRes(L"PointMesh", pMesh);

	// ==============
	// RectMesh 생성
	// ==============

	// 0 --- 1 
	// |  \  |
	// 3 --- 2

	v.vPos = Vec3(-0.5f, 0.5f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vUV = Vec2(0.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, 0.5f, 0.f);
	v.vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	v.vUV = Vec2(1.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, -0.5f, 0.f);
	v.vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	v.vUV = Vec2(1.f, 1.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(-0.5f, -0.5f, 0.f);
	v.vColor = Vec4(0.f, 0.f, 0.f, 1.f);
	v.vUV = Vec2(0.f, 1.f);
	vecVtx.push_back(v);

	vecIdx.push_back(0);
	vecIdx.push_back(2);
	vecIdx.push_back(3);

	vecIdx.push_back(0);
	vecIdx.push_back(1);
	vecIdx.push_back(2);

	// ======== 일반 RectMesh 저장
	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddRes(L"RectMesh", pMesh);

	// ======== Debug용 RectMesh 저장
	vecIdx.clear();
	vecIdx.push_back(0);
	vecIdx.push_back(1);
	vecIdx.push_back(2);
	vecIdx.push_back(3);
	vecIdx.push_back(0);

	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddRes(L"RectMesh_Debug", pMesh);

	vecVtx.clear();
	vecIdx.clear();

	// =================
	// Circle Mesh
	// =================
	
	//      3 
	//   4     2		// 반지름이 1인 원 : 단위원
	//  5 --0-- 1		// 원점에서 θ만큼 움직였을 때의 좌표는 (cos(θ), sin(θ))
	// 반지름			// 반지름이 r인 원이라면 좌표값은 (r * cos(θ), r * sin(θ))
	
	// 원의 중심점
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vUV = Vec2(0.5f, 0.5f);
	vecVtx.push_back(v);

	// 반지름
	float fRadius = 0.5f;

	// 각도
	UINT  Slice = 40;							 // 원을 몇 조각으로 나눌 것인지
	float fTheta = XM_2PI / (float) Slice;		 // 2PI : 단위원의 둘레, 즉 단위원을 fSlice 조각 갯수로 나눈 각도

	// 정점 위치 지정
	for (UINT i = 0; i < Slice; i++)
	{
		v.vPos = Vec3(Vec3(fRadius * cosf(fTheta * (float)i), fRadius * sinf(fTheta * (float)i), 0.f));
		v.vUV = Vec2(v.vPos.x + 0.5f, -v.vPos.y + 0.5f);
		vecVtx.push_back(v);
	}

	// 인덱스 설정
	for (UINT i = 0; i < Slice - 1; i++)		// 마지막 점은 예외처리로 지정해주기 위해 반복문을 Slice -1 번 순회한다
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 2);
		vecIdx.push_back(i + 1);
	}

	// 마지막 삼각형은 원의 중심점을 제외한 첫 번째 정점(1)과 연결되어야 하므로 예외처리로 따로 작성해준다.
	vecIdx.push_back(0);
	vecIdx.push_back(1);
	vecIdx.push_back(Slice);
	
	// 일반 CircleMesh 저장
	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddRes(L"CircleMesh", pMesh);

	// Debug 용 CircleMesh 저장
	vecIdx.clear();

	for (UINT i = 0; i < Slice; i++)		
	{
		vecIdx.push_back(i + 1);
	}
	vecIdx.push_back(1);
	
	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddRes(L"CircleMesh_Debug", pMesh);

	vecVtx.clear();
	vecIdx.clear();

	// =================
	// Debug Line Mesh
	// =================

	// 라인용의 정점 두개를 생성
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vUV = Vec2(0.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(1.f, 0.f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vUV = Vec2(1.f, 0.f);
	vecVtx.push_back(v);

	// 인덱스버퍼는 굳이 필요없긴 하지만 있으니까 사용
	vecIdx.push_back(0);
	vecIdx.push_back(1);

	// Debug LineMesh 저장
	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddRes(L"LineMesh_Debug", pMesh);

	vecVtx.clear();
	vecIdx.clear();
}

void CResourceManager::CreateDefaultGraphicsShader()
{
	// Shader 생성
	Ptr<CGraphicsShader> pShader = nullptr;

	// ============
	// Test Shader
	// ============

	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\test.fx", "VS_Test");
	pShader->CreatePixelShader(L"shader\\test.fx", "PS_Test");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_WRITE);		// 반투명한 재질은 깊이 테스트는 진행하나 (가려질 수는 있으나) 깊이는 기록하지 않는다 (남을 가리지 않는다)
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);	// No_Write, AlphaBlend 사용 도메인은 무조건 반투명(transparent) 도메인

	// Param
	pShader->AddTexParam(TEX_0, "Output Texture");

	// ResourceManager의 map 배열에 추가
	AddRes(L"TestShader", pShader);
	

	// =======================================
	//				Std2D Shader
	// 
	// RasterizerState        : None
	// BlendState             : Mask
	// DepthStencilState      : Less
	//
	//				 Parameter
	// g_tex_0                : Output Texture
	// ========================================

	pShader = new CGraphicsShader;
	pShader->SetKey(L"Std2DShader");
	pShader->CreateVertexShader(L"shader\\std2d.fx", "VS_Std2D");
	pShader->CreatePixelShader(L"shader\\std2d.fx", "PS_Std2D");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::MASK);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);

	// Param
	pShader->AddScalarParam(INT_1, "Test Int");
	pShader->AddScalarParam(FLOAT_0, "Test Float");
	pShader->AddScalarParam(VEC2_0, "Test Vec2");
	pShader->AddScalarParam(VEC4_0, "Test Vec4");

	pShader->AddTexParam(TEX_0, "Output Texture 1");
	pShader->AddTexParam(TEX_1, "Output Texture 2");
	pShader->AddTexParam(TEX_2, "Output Texture 3");

	// ResourceManager의 map 배열에 추가
	AddRes(pShader->GetKey(), pShader);

	// ======================================
	// Std2DLightShader
	// RasterizerState      : None
	// BlendState           : Mask
	// DepthStencilState    : Less
	//
	// Parameter
	// g_tex_0              : Output Texture
	// g_tex_1              : Nomal Texture
	// ======================================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"Std2DLightShader");
	pShader->CreateVertexShader(L"shader\\std2d.fx", "VS_Std2DLight");
	pShader->CreatePixelShader(L"shader\\std2d.fx", "PS_Std2DLight");

	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	//pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);
	//pShader->SetBSType(BS_TYPE::MASK);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);
	//pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);
	pShader->AddScalarParam(INT_1, "Test Int");
	pShader->AddScalarParam(VEC4_0, "Test Vec4");

	pShader->AddTexParam(TEX_0, "Output Texture 1");
	pShader->AddTexParam(TEX_1, "Output Texture 2");
	pShader->AddTexParam(TEX_2, "Output Texture 3");

	AddRes(pShader->GetKey(), pShader);

	// =========================
	// DebugShape Shader
	// 
	// Tolpology  : LineStrip 
	// RS_TYPE    : CULL_NONE
	// DS_TYPE    : NO_TEST_NO_WRITE 
	// BS_TYPE    : Defalut          
	//
	// g_vec4_0 : OutColor 
	// =========================

	pShader = new CGraphicsShader;
	pShader->SetKey(L"DebugShapeShader");
	pShader->CreateVertexShader(L"shader\\debugshape.fx", "VS_DebugShape");
	pShader->CreatePixelShader(L"shader\\debugshape.fx", "PS_DebugShape");
	
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);

	AddRes(pShader->GetKey(), pShader);

	// ============================
	// TileMap Shader
	// 
	// RS_TYPE : CULL_NONE
	// DS_TYPE : LESS
	// BS_TYPE : MASK

	// Parameter
	// g_tex_0 : Tile Atlas Texture
	// ============================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"TileMapShader");
	pShader->CreateVertexShader(L"shader\\tilemap.fx", "VS_TileMap");
	pShader->CreatePixelShader(L"shader\\tilemap.fx", "PS_TileMap");

	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::MASK);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);

	AddRes(pShader->GetKey(), pShader);

	// ============================
	// ParticleRender
	// 
	// RS_TYPE : CULL_NONE
	// DS_TYPE : NO_WRITE
	// BS_TYPE : ALPHA_BLEND
	
	// Parameter
	// g_int_0 : Particle Index
	// 
	// Domain : TRANSPARENT
	// ============================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"ParticleRenderShader");
	pShader->CreateVertexShader(L"shader\\particle_render.fx", "VS_ParticleRender");
	pShader->CreateGeometryShader(L"shader\\particle_render.fx", "GS_ParticleRender");
	pShader->CreatePixelShader(L"shader\\particle_render.fx", "PS_ParticleRender");

	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);

	// 파티클렌더 셰이더는 Point Mesh를 사용하므로, Topology 설정을 Pointlist로 수정한다.
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);

	AddRes(pShader->GetKey(), pShader);

	// ============================
	// GrayShader
	// RS_TYPE : CULL_NONE
	// DS_TYPE : NO_TEST_NO_WRITE (화면 전체를 그대로 덮어버리는 과정으로, 깊이 테스트/기록 X)
	// BS_TYPE : DEFAULT		  (덮어버리는 과정이므로 블렌딩 자체가 필요 없음)
	// Domain : POSTPROCESS
	// ============================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"GrayShader");
	pShader->CreateVertexShader(L"shader\\postprocess.fx", "VS_GrayShader");
	pShader->CreatePixelShader(L"shader\\postprocess.fx", "PS_GrayShader");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);
	AddRes(pShader->GetKey(), pShader);

	// ============================
	// Distortion Shader
	// RS_TYPE : CULL_NONE
	// DS_TYPE : NO_TEST_NO_WRITE
	// BS_TYPE : DEFAULT	 
	// Domain : POSTPROCESS
	// ============================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"DistortionShader");
	pShader->CreateVertexShader(L"shader\\postprocess.fx", "VS_Distortion");
	pShader->CreatePixelShader(L"shader\\postprocess.fx", "PS_Distortion");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);

	// Parameter
	pShader->AddTexParam(TEX_1, "Noise Texture");

	AddRes(pShader->GetKey(), pShader);

	// =======================================
	//				Std2D Shader
	// 
	// RasterizerState        : None
	// BlendState             : Mask
	// DepthStencilState      : Less
	//
	//				 Parameter
	// g_tex_0                : Output Texture
	// ========================================

	pShader = new CGraphicsShader;
	pShader->SetKey(L"Std2DBloodShader");
	pShader->CreateVertexShader(L"shader\\std2d.fx", "VS_Std2D_Blood");
	pShader->CreatePixelShader(L"shader\\std2d.fx", "PS_Std2D_Blood");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetBSType(BS_TYPE::MASK);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_MASK);
	AddRes(pShader->GetKey(), pShader);

	// =======================================
	//				Std2D_AlphaShader
	// 
	// RasterizerState        : None
	// BlendState             : NO_Write
	// DepthStencilState      : Alpha_blend
	//
	//				 Parameter
	// g_int_0				  : Use Overlay
	// g_float_0			  : Overlay Alpha
	// g_vec4_0				  :	Overlay Color
	// ========================================

	pShader = new CGraphicsShader;
	pShader->SetKey(L"Std2DAlphaShader");
	pShader->CreateVertexShader(L"shader\\std2d.fx", "VS_Std2D_Alpha");
	pShader->CreatePixelShader(L"shader\\std2d.fx", "PS_Std2D_Alpha");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);

	pShader->AddScalarParam(FLOAT_0, "Test float");
	pShader->AddTexParam(TEX_0, "Output Texture 1");

	AddRes(pShader->GetKey(), pShader);

	// =======================================
//				Std2D_AlphaShader
// 
// RasterizerState        : None
// BlendState             : NO_Write
// DepthStencilState      : Alpha_blend
//
//				 Parameter
// g_tex_0				  : Red-dot
// g_tex_1				  : Mask
// ========================================

	pShader = new CGraphicsShader;
	pShader->SetKey(L"Std2DLaserShader");
	pShader->CreateVertexShader(L"shader\\std2d.fx", "VS_Std2D_Laser");
	pShader->CreatePixelShader(L"shader\\std2d.fx", "PS_Std2D_Laser");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);

	pShader->AddTexParam(TEX_0, "Output Texture 1");
	pShader->AddTexParam(TEX_1, "Output Texture 2");

	AddRes(pShader->GetKey(), pShader);

	// ============================
	// GlitchShader
	// RS_TYPE : CULL_NONE
	// DS_TYPE : NO_TEST_NO_WRITE (화면 전체를 그대로 덮어버리는 과정으로, 깊이 테스트/기록 X)
	// BS_TYPE : DEFAULT		  (덮어버리는 과정이므로 블렌딩 자체가 필요 없음)
	// Domain : POSTPROCESS
	// ============================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"GlitchShader");
	pShader->CreateVertexShader(L"shader\\postprocess.fx", "VS_Glitch");
	pShader->CreatePixelShader(L"shader\\postprocess.fx", "PS_Glitch");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);
	AddRes(pShader->GetKey(), pShader);

	// ============================
	// New Distortion Shader
	// RS_TYPE : CULL_NONE
	// DS_TYPE : NO_TEST_NO_WRITE (화면 전체를 그대로 덮어버리는 과정으로, 깊이 테스트/기록 X)
	// BS_TYPE : DEFAULT		  (덮어버리는 과정이므로 블렌딩 자체가 필요 없음)
	// Domain : POSTPROCESS
	// ============================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"NewDistortionShader");
	pShader->CreateVertexShader(L"shader\\postprocess.fx", "VS_NewDistortion");
	pShader->CreatePixelShader(L"shader\\postprocess.fx", "PS_NewDistortion");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_TEST_NO_WRITE);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_POSTPROCESS);
	AddRes(pShader->GetKey(), pShader);

	// =======================================
	//			Std2D_UI Shader
	// 
	// RasterizerState        : None
	// BlendState             : Mask
	// DepthStencilState      : Less
	//
	//				 Parameter
	// g_int_0				  : Timer Only
	// g_float_0			  : Time Percentage
	// g_tex_0                : Output Texture
	// ========================================
	pShader = new CGraphicsShader;
	pShader->SetKey(L"Std2D_UIShader");
	pShader->CreateVertexShader(L"shader\\std2d.fx", "VS_Std2D_UI");
	pShader->CreatePixelShader(L"shader\\std2d.fx", "PS_Std2D_UI");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetBSType(BS_TYPE::MASK);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_UI);
	pShader->AddTexParam(TEX_0, "Output Texture 1");

	pShader->AddScalarParam(INT_0, "Use Masking");
	pShader->AddScalarParam(FLOAT_0, "Percentage");
	// ResourceManager의 map 배열에 추가
	AddRes(pShader->GetKey(), pShader);

	// =======================================
	//				Std2D_FadeInOutShader
	// 
	// RasterizerState        : None
	// BlendState             : NO_Write
	// DepthStencilState      : Alpha_blend
	//
	//				 Parameter
	// g_int_0				  : Use Overlay
	// g_float_0			  : Overlay Alpha
	// ========================================

	pShader = new CGraphicsShader;
	pShader->SetKey(L"Std2DFadeInOutShader");
	pShader->CreateVertexShader(L"shader\\std2d.fx", "VS_Std2D_Alpha");
	pShader->CreatePixelShader(L"shader\\std2d.fx", "PS_Std2D_Alpha");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_WRITE);
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_UI);

	pShader->AddScalarParam(FLOAT_0, "Test float");
	pShader->AddScalarParam(INT_0, "Use Overlay");
	pShader->AddTexParam(TEX_0, "Output Texture 1");

	AddRes(pShader->GetKey(), pShader);
}

void CResourceManager::CreateDefaultComputeShader()
{
	Ptr<CComputeShader> pCS = nullptr;

	// Texture 색상 변경 쉐이더
	pCS = new CSetColorShader(32,32,1);
	pCS->SetKey(L"SetColorCS");
	pCS->CreateComputeShader(L"shader\\setcolor.fx", "CS_SetColor");
	AddRes(pCS->GetKey(), pCS);

	// Particle Update 쉐이더
	pCS = new CParticleUpdateShader(128, 1, 1);
	pCS->SetKey(L"ParticleUpdateCS");
	pCS->CreateComputeShader(L"shader\\particle_update.fx", "CS_ParticleUpdate");
	AddRes(pCS->GetKey(), pCS);
}

void CResourceManager::CreateDefaultMaterial()
{
	// 이 함수에서 만들어지는 재질들은 엔진에서 만들어진 재질로, 세이브로드가 불가능하다
	// (늘 빌드 시 초기화되므로 세이브/로드의 의미가 없다, 따라서 생성자에 true)

	Ptr<CMaterial> pMtrl = nullptr;

	// DebugShape Material
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"DebugShapeShader"));
	AddRes(L"DebugShapeMtrl", pMtrl);

	// Test Material
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"TestShader"));
	AddRes(L"TestMtrl", pMtrl);

	// Std2D Material
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"Std2DShader"));
	AddRes(L"Std2DMtrl", pMtrl);

	// Std2D Animation Material
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"Std2DShader"));
	AddRes(L"Std2DAnimMtrl", pMtrl);

	// Std2DLight Material
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"Std2DLightShader"));
	AddRes(L"Std2DLightMtrl", pMtrl);

	// Std2DLight Animation Material
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"Std2DLightShader"));
	AddRes(L"Std2DAnimLightMtrl", pMtrl);

	// TileMap Material
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"TileMapShader"));
	AddRes(L"TileMapMtrl", pMtrl);

	// Particle Render Material
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"ParticleRenderShader"));
	AddRes(L"ParticleRenderMtrl", pMtrl);

	// Particle Render Material
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"ParticleRenderShader"));
	AddRes(L"ParticleTexRenderMtrl", pMtrl);

	// GrayShader(PostProcess)
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"GrayShader"));
	AddRes(L"GrayMtrl", pMtrl);

	// DistortionShader(PostProcess)
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"DistortionShader"));
	AddRes(L"DistortionMtrl", pMtrl);

	// GlitchShader (PostProcess)
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"GlitchShader"));
	AddRes(L"GlitchMtrl", pMtrl);

	// GlitchShader (PostProcess)
	pMtrl = new CMaterial(true);
	pMtrl->SetShader(FindRes<CGraphicsShader>(L"NewDistortionShader"));
	AddRes(L"NewDistortionMtrl", pMtrl);
}



void CResourceManager::LoadDefaultTexture()
{
	Load<CTexture>(L"texture\\milk.png", L"texture\\milk.png");
	Load<CTexture>(L"texture\\smokeparticle.png", L"texture\\smokeparticle.png");
	Load<CTexture>(L"texture\\beheaded.png", L"texture\\beheaded.png");
	Load<CTexture>(L"texture\\beheaded_n.png", L"texture\\beheaded_n.png");
	Load<CTexture>(L"texture\\TILE.bmp", L"texture\\TILE.bmp");
	Load<CTexture>(L"texture\\link.png", L"texture\\link.png");

	// NoiseTexture
	Load<CTexture>(L"texture\\Noise\\noise_01.png", L"texture\\Noise\\noise_01.png");
	Load<CTexture>(L"texture\\Noise\\noise_02.png", L"texture\\Noise\\noise_02.png");
	Load<CTexture>(L"texture\\Noise\\noise_03.jpg", L"texture\\Noise\\noise_03.jpg");
}

Ptr<CTexture> CResourceManager::CreateTexture(const wstring& _strKey, UINT _Width, UINT _Height, DXGI_FORMAT _pixelformat, UINT _BindFlag, D3D11_USAGE _Usage)
{
	// 1. 만들려고 하는 텍스처의 키값으로 검색하여 중복키 체크 -> 중복 있을 시 assert
	Ptr<CTexture> pTex = FindRes<CTexture>(_strKey);
	assert(nullptr == pTex);

	// 2. CTexture의 CreateTexture함수 호출하여 새 텍스처 생성 후, 리소스 맵에 등록 후 반환
	// 이때 이 함수를 통해 만들어지는 텍스처는 엔진 리소스이므로, 생성자에 true
	pTex = new CTexture(true);
	if (FAILED(pTex->Create(_Width, _Height, _pixelformat, _BindFlag, _Usage)))
	{
		assert(nullptr);
	}

	AddRes<CTexture>(_strKey, pTex);

	return pTex;
}

Ptr<CTexture> CResourceManager::CreateTexture(const wstring& _strKey, ComPtr<ID3D11Texture2D> _Tex2D)
{
	// 1. 만들려고 하는 텍스처의 키값으로 검색하여 중복키 체크 -> 중복 있을 시 assert
	Ptr<CTexture> pTex = FindRes<CTexture>(_strKey);
	assert(nullptr == pTex);

	// 2. CTexture의 CreateTexture함수 호출하여 새 텍스처 생성 후, 리소스 맵에 등록 후 반환
	pTex = new CTexture;
	if (FAILED(pTex->Create(_Tex2D)))
	{
		assert(nullptr);
	}

	AddRes<CTexture>(_strKey, pTex);

	return pTex;
}

void CResourceManager::DeleteTexture(const wstring& _strKey)
{
	// 1. 삭제할 텍스처의 키값으로 검색 -> 해당 텍스처 없을 경우 assert
	Ptr<CTexture> pTex = FindRes<CTexture>(_strKey);
	assert(pTex != nullptr);

	// 2. 리소스 목록에서 해당 텍스처 삭제
	m_arrRes[(UINT)RES_TYPE::TEXTURE].erase(_strKey);

	// 3. 해당 텍스처 삭제
	delete pTex.Get();

	m_Changed = true;
}

void CResourceManager::DeleteTexture(Ptr<CTexture> _Tex)
{
	// 1. 삭제할 텍스처의 키값으로 검색 -> 해당 텍스처 없을 경우 assert
	Ptr<CTexture> pTex = FindRes<CTexture>(_Tex->GetKey());
	assert(pTex != nullptr);

	// 2. 리소스 목록에서 해당 텍스처 삭제
	m_arrRes[(UINT)RES_TYPE::TEXTURE].erase(_Tex->GetKey());

	// 3. 해당 텍스처 삭제
	delete pTex.Get();
	pTex = nullptr;

	m_Changed = true;
}

void CResourceManager::DeleteRes(RES_TYPE _type, const wstring& _strKey)
{
	// 리소스 맵에서 키값으로 해당 리소스를 찾는다 -> 리소스가 없으면 assert 
	map<wstring, Ptr<CResource>>::iterator iter = m_arrRes[(UINT)_type].find(_strKey);
	assert(!(iter == m_arrRes[(UINT)_type].end()));

	// 맵에서 해당 리소스를 삭제한다
	// 스마트 포인터로 관리되기 때문에, 참조카운트가 0이 되는 순간 스스로 delete 될 것
	m_arrRes[(UINT)_type].erase(iter);

	m_Changed = true;
}
