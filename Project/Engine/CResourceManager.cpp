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
	// ������� ���θ� false�� �ʱ�ȭ
	m_Changed = false;
}

void CResourceManager::InitSound()
{
	FMOD::System_Create(&CSound::g_pFMOD);

	if (nullptr == CSound::g_pFMOD)
	{
		assert(nullptr);
	}

	// 32�� ä�� ����
	CSound::g_pFMOD->init(32, FMOD_DEFAULT, nullptr);
}

void CResourceManager::CreateDefaultMesh()
{
	vector<Vtx> vecVtx;
	vector<UINT> vecIdx;
	Vtx v;

	Ptr<CMesh> pMesh = nullptr;
	
	// ========================
	// PointMesh ����(��ƼŬ��)
	// ========================
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);

	UINT idx = 0;

	pMesh = new CMesh(true);
	pMesh->Create(&v, 1, &idx, 1);
	AddRes(L"PointMesh", pMesh);

	// ==============
	// RectMesh ����
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

	// ======== �Ϲ� RectMesh ����
	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddRes(L"RectMesh", pMesh);

	// ======== Debug�� RectMesh ����
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
	//   4     2		// �������� 1�� �� : ������
	//  5 --0-- 1		// �������� �踸ŭ �������� ���� ��ǥ�� (cos(��), sin(��))
	// ������			// �������� r�� ���̶�� ��ǥ���� (r * cos(��), r * sin(��))
	
	// ���� �߽���
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vUV = Vec2(0.5f, 0.5f);
	vecVtx.push_back(v);

	// ������
	float fRadius = 0.5f;

	// ����
	UINT  Slice = 40;							 // ���� �� �������� ���� ������
	float fTheta = XM_2PI / (float) Slice;		 // 2PI : �������� �ѷ�, �� �������� fSlice ���� ������ ���� ����

	// ���� ��ġ ����
	for (UINT i = 0; i < Slice; i++)
	{
		v.vPos = Vec3(Vec3(fRadius * cosf(fTheta * (float)i), fRadius * sinf(fTheta * (float)i), 0.f));
		v.vUV = Vec2(v.vPos.x + 0.5f, -v.vPos.y + 0.5f);
		vecVtx.push_back(v);
	}

	// �ε��� ����
	for (UINT i = 0; i < Slice - 1; i++)		// ������ ���� ����ó���� �������ֱ� ���� �ݺ����� Slice -1 �� ��ȸ�Ѵ�
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 2);
		vecIdx.push_back(i + 1);
	}

	// ������ �ﰢ���� ���� �߽����� ������ ù ��° ����(1)�� ����Ǿ�� �ϹǷ� ����ó���� ���� �ۼ����ش�.
	vecIdx.push_back(0);
	vecIdx.push_back(1);
	vecIdx.push_back(Slice);
	
	// �Ϲ� CircleMesh ����
	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddRes(L"CircleMesh", pMesh);

	// Debug �� CircleMesh ����
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

	// ���ο��� ���� �ΰ��� ����
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vUV = Vec2(0.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(1.f, 0.f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vUV = Vec2(1.f, 0.f);
	vecVtx.push_back(v);

	// �ε������۴� ���� �ʿ���� ������ �����ϱ� ���
	vecIdx.push_back(0);
	vecIdx.push_back(1);

	// Debug LineMesh ����
	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	AddRes(L"LineMesh_Debug", pMesh);

	vecVtx.clear();
	vecIdx.clear();
}

void CResourceManager::CreateDefaultGraphicsShader()
{
	// Shader ����
	Ptr<CGraphicsShader> pShader = nullptr;

	// ============
	// Test Shader
	// ============

	pShader = new CGraphicsShader;
	pShader->CreateVertexShader(L"shader\\test.fx", "VS_Test");
	pShader->CreatePixelShader(L"shader\\test.fx", "PS_Test");
	pShader->SetRSType(RS_TYPE::CULL_NONE);
	pShader->SetDSType(DS_TYPE::NO_WRITE);		// �������� ������ ���� �׽�Ʈ�� �����ϳ� (������ ���� ������) ���̴� ������� �ʴ´� (���� ������ �ʴ´�)
	pShader->SetBSType(BS_TYPE::ALPHA_BLEND);

	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);	// No_Write, AlphaBlend ��� �������� ������ ������(transparent) ������

	// Param
	pShader->AddTexParam(TEX_0, "Output Texture");

	// ResourceManager�� map �迭�� �߰�
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

	// ResourceManager�� map �迭�� �߰�
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

	// ��ƼŬ���� ���̴��� Point Mesh�� ����ϹǷ�, Topology ������ Pointlist�� �����Ѵ�.
	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_TRANSPARENT);

	AddRes(pShader->GetKey(), pShader);

	// ============================
	// GrayShader
	// RS_TYPE : CULL_NONE
	// DS_TYPE : NO_TEST_NO_WRITE (ȭ�� ��ü�� �״�� ��������� ��������, ���� �׽�Ʈ/��� X)
	// BS_TYPE : DEFAULT		  (��������� �����̹Ƿ� ���� ��ü�� �ʿ� ����)
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
	// DS_TYPE : NO_TEST_NO_WRITE (ȭ�� ��ü�� �״�� ��������� ��������, ���� �׽�Ʈ/��� X)
	// BS_TYPE : DEFAULT		  (��������� �����̹Ƿ� ���� ��ü�� �ʿ� ����)
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
	// DS_TYPE : NO_TEST_NO_WRITE (ȭ�� ��ü�� �״�� ��������� ��������, ���� �׽�Ʈ/��� X)
	// BS_TYPE : DEFAULT		  (��������� �����̹Ƿ� ���� ��ü�� �ʿ� ����)
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
	// ResourceManager�� map �迭�� �߰�
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

	// Texture ���� ���� ���̴�
	pCS = new CSetColorShader(32,32,1);
	pCS->SetKey(L"SetColorCS");
	pCS->CreateComputeShader(L"shader\\setcolor.fx", "CS_SetColor");
	AddRes(pCS->GetKey(), pCS);

	// Particle Update ���̴�
	pCS = new CParticleUpdateShader(128, 1, 1);
	pCS->SetKey(L"ParticleUpdateCS");
	pCS->CreateComputeShader(L"shader\\particle_update.fx", "CS_ParticleUpdate");
	AddRes(pCS->GetKey(), pCS);
}

void CResourceManager::CreateDefaultMaterial()
{
	// �� �Լ����� ��������� �������� �������� ������� ������, ���̺�ε尡 �Ұ����ϴ�
	// (�� ���� �� �ʱ�ȭ�ǹǷ� ���̺�/�ε��� �ǹ̰� ����, ���� �����ڿ� true)

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
	// 1. ������� �ϴ� �ؽ�ó�� Ű������ �˻��Ͽ� �ߺ�Ű üũ -> �ߺ� ���� �� assert
	Ptr<CTexture> pTex = FindRes<CTexture>(_strKey);
	assert(nullptr == pTex);

	// 2. CTexture�� CreateTexture�Լ� ȣ���Ͽ� �� �ؽ�ó ���� ��, ���ҽ� �ʿ� ��� �� ��ȯ
	// �̶� �� �Լ��� ���� ��������� �ؽ�ó�� ���� ���ҽ��̹Ƿ�, �����ڿ� true
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
	// 1. ������� �ϴ� �ؽ�ó�� Ű������ �˻��Ͽ� �ߺ�Ű üũ -> �ߺ� ���� �� assert
	Ptr<CTexture> pTex = FindRes<CTexture>(_strKey);
	assert(nullptr == pTex);

	// 2. CTexture�� CreateTexture�Լ� ȣ���Ͽ� �� �ؽ�ó ���� ��, ���ҽ� �ʿ� ��� �� ��ȯ
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
	// 1. ������ �ؽ�ó�� Ű������ �˻� -> �ش� �ؽ�ó ���� ��� assert
	Ptr<CTexture> pTex = FindRes<CTexture>(_strKey);
	assert(pTex != nullptr);

	// 2. ���ҽ� ��Ͽ��� �ش� �ؽ�ó ����
	m_arrRes[(UINT)RES_TYPE::TEXTURE].erase(_strKey);

	// 3. �ش� �ؽ�ó ����
	delete pTex.Get();

	m_Changed = true;
}

void CResourceManager::DeleteTexture(Ptr<CTexture> _Tex)
{
	// 1. ������ �ؽ�ó�� Ű������ �˻� -> �ش� �ؽ�ó ���� ��� assert
	Ptr<CTexture> pTex = FindRes<CTexture>(_Tex->GetKey());
	assert(pTex != nullptr);

	// 2. ���ҽ� ��Ͽ��� �ش� �ؽ�ó ����
	m_arrRes[(UINT)RES_TYPE::TEXTURE].erase(_Tex->GetKey());

	// 3. �ش� �ؽ�ó ����
	delete pTex.Get();
	pTex = nullptr;

	m_Changed = true;
}

void CResourceManager::DeleteRes(RES_TYPE _type, const wstring& _strKey)
{
	// ���ҽ� �ʿ��� Ű������ �ش� ���ҽ��� ã�´� -> ���ҽ��� ������ assert 
	map<wstring, Ptr<CResource>>::iterator iter = m_arrRes[(UINT)_type].find(_strKey);
	assert(!(iter == m_arrRes[(UINT)_type].end()));

	// �ʿ��� �ش� ���ҽ��� �����Ѵ�
	// ����Ʈ �����ͷ� �����Ǳ� ������, ����ī��Ʈ�� 0�� �Ǵ� ���� ������ delete �� ��
	m_arrRes[(UINT)_type].erase(iter);

	m_Changed = true;
}
