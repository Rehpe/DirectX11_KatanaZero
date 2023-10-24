#pragma once

struct tVertex
{
	Vec3 vPos;			//��ǥ ����
	Vec4 vColor;		//���� ����
	Vec2 vUV;
};

typedef tVertex Vtx;


// Event ����ü
struct tEvent
{
	EVENT_TYPE	Type;
	DWORD_PTR	wParam;
	DWORD_PTR	lParam;
};

struct tPlatformColliderInfo
{
	PLATFORM_TYPE	Type;
	int				Priority;	// ���� ���� �켱����

	// RAMP�� ���
	Vec3			vStartPos;
	Vec3			vEndPos;

};

struct tGameObjData
{
	int		ID;
	wstring	ObjName;
	Vec3	Position;
	Vec3	Scale;
	Vec3	Rotation;
	bool	AnimUse;
	wstring	CurAnimName;
	int		CurAnimFrm;
	bool	LightUse;
	Vec3	LightDiffuse;
	bool	MeshRenderUse;
	wstring MtrlName;
	bool	IsNewlyCreated;
	bool	IsDead;
};

// DebugFont ����ü
struct tDebugFont
{
	const wchar_t* szBuff;
	float PosX;
	float PosY;
	float FontSize;
	UINT  Color;
};

// DebugShape ����ü
struct tDebugShapeInfo
{
	SHAPE_TYPE  eShape;			// ���
	Vec3		vWorldPos;		// ��ġ
	Vec3		vWorldScale;	// ũ��
	Vec3		vWorldRot;		// ȸ������
	Matrix		matWorld;		// ��ġ.ũ��.ȸ���� ��Ʈ������ ���� ��츦 ���, 3��ҷ� ���� ��� �������
	Vec3		vStartPos;
	Vec3		vEndPos;
	Vec4		vColor;			// ����
	float		fMaxTime;		// ������ �ð�
	float		fCurTime;		// ���� �ð�
};


struct tLightColor
{
	Vec4 vDiffuse;	// ���� ����
	Vec4 vAmbient;	// �ֺ� ��(ȯ�� ��)
};

// LightInfo
struct tLightInfo
{
	tLightColor Color;		 // ���� ����

	Vec4		vWorldPos;   // ������ ���� �����̽� ��ġ
	Vec4		vWorldDir;	 // ���� ������ ����

	UINT		LightType;   // ���� Ÿ��(���⼺, ��, ����Ʈ) -> ������ۿ� ���ε��ϱ� ���Ͽ� enum�� �ƴ� UINT
	float		Radius;		 // ���� �ݰ�(��Ÿ�)
	float		Angle;		 // ���� ����
	float		Intensity;	 
};

// TileMap
struct tTile
{
	Vec2 vLeftTop;		// Ÿ�� �»�� 
	Vec2 vSlice;		// Ÿ�� ũ��
};

struct tTileMapIdx
{
	int	TileIdx;
	int	Padding;
	int	Padding2;
	int	Padding3;
};

// Animator2D
struct tAnim2DFrm
{
	Vec2	LeftTopUV;
	Vec2	SliceUV;
	Vec2	OffsetUV;
	float	fDuration;
};

// Particle
struct tParticle
{
	Vec4	vLocalPos;		// ������Ʈ�κ��� ������ �Ÿ�
	Vec4	vWorldPos;		// ��ƼŬ ��ġ
	Vec4	vWorldScale;	// ��ƼŬ ũ��
	Vec4	vColor;			// ��ƼŬ ����
	Vec4	vVelocity;		// ��ƼŬ ���� �ӵ�
	Vec4	vForce;			// ��ƼŬ�� �־��� ��
	Vec4	vRandomForce;	// ��ƼŬ�� ����Ǵ� �������� ��

	float   Age;			// ���� �ð�
	float   PrevAge;		// ���� ������ ���� �ð�
	float   NomalizedAge;	// ������ �����ð��� 0~1�� ����ȭ �� ��
	float	LifeTime;		// ����
	float	Mass;			// ����
	float   ScaleFactor;	// �߰� ũ�� ����

	int     Active;			// ��ƼŬ Ȱ��ȭ ����
	int     pad;
};

struct tRWParticleBuffer
{
	int		SpawnCount;			// ���� ��ų ��ƼŬ ����
	int		padding[3];
};

struct tParticleModule
{
	// ���� ���
	Vec4    vSpawnColor;
	Vec4	vSpawnScaleMin;
	Vec4	vSpawnScaleMax;
	Vec3	vBoxShapeScale;
	float	fSphereShapeRadius;
	int		SpawnShapeType;		// 0 : BOX, 1 : Sphere
	int		SpawnRate;			// �ʴ� ���� ����
	int		Space;				// ��ƼŬ ������Ʈ ��ǥ�� ( 0 : World,  1 : Local)
	float   MinLifeTime;		// �ּ� ����
	float   MaxLifeTime;		// �ִ� ����
	int     spawnpad[3];

	// Color Change ���
	Vec4	vStartColor;		// �ʱ� ����
	Vec4	vEndColor;			// ���� ����

	// Scale Change ���
	float	StartScale;		// �ʱ� ũ��
	float	EndScale;			// ���� ũ��	

	// ���� �ִ�ũ��
	int		iMaxParticleCount;
	int		ipad;

	// Add Velocity ���
	Vec4	vVelocityDir;
	int     AddVelocityType;	// 0 : From Center, 1: To Center, 2 : Fixed Direction	
	float	OffsetAngle;
	float	Speed;
	int     addvpad;

	// Drag ��� - �ӵ� ����
	float	StartDrag;
	float	EndDrag;

	// NoiseForce ��� - ���� �� ����	
	float	fNoiseTerm;		// ���� �� ���� ����
	float	fNoiseForce;	// ���� �� ũ��

	// Render ���
	int		VelocityAlignment;	// 1 : �ӵ����� ���(�̵� �������� ȸ��) 0 : ��� ����
	int		VelocityScale;		// 1 : �ӵ��� ���� ũ�� ��ȭ ���, 0 : ��� ����	
	float   vMaxSpeed;			// �ִ� ũ�⿡ �����ϴ� �ӷ�
	Vec4	vMaxVelocityScale;	// �ӷ¿� ���� ũ�� ��ȭ�� �ִ�ġ
	int		renderpad;

	// Module Check	(����� ������� ������ ���� ����)
	int		ModuleCheck[(UINT)PARTICLE_MODULE::END];
};


// =======================
// ��� ���� ���� ����ü
// =======================

struct tTransform		// ��� ���۸� ���� ������ ������Ʈ�� ��� ����
{
	Matrix matWorld;
	Matrix matView;
	Matrix matProj;

	Matrix matWV;		// World ��� * View ���
	Matrix matWVP;		// World ��� * View ��� * Projection ���
};

extern tTransform g_transform;


struct tMtrlConst		// ������ ���� ������ ������� Ÿ�Ժ��� 4���� ���� + �ؽ�ó �������Ϳ� �ؽ�ó�� ���ε� �Ǿ����� ���� 8��
{
	int arrInt[4];
	float arrFloat[4];
	Vec2 arrV2[4];
	Vec4 arrV4[4];
	Matrix arrMat[4];

	int arrTex[(UINT)TEX_PARAM::TEX_END];
};

struct tGlobal			// ���� ��� ���ۿ��� ����ϴ� ����ü
{
	Vec2  Resolution;
	float tDT;
	float tAccTime;
	UINT  Light2DCount;
	UINT  Light3DCount;
	int	  globalpadding[2];
};

extern tGlobal GlobalData; // �������� ����� �� �ְ� extern ����