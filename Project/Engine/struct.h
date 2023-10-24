#pragma once

struct tVertex
{
	Vec3 vPos;			//좌표 정보
	Vec4 vColor;		//색상 정보
	Vec2 vUV;
};

typedef tVertex Vtx;


// Event 구조체
struct tEvent
{
	EVENT_TYPE	Type;
	DWORD_PTR	wParam;
	DWORD_PTR	lParam;
};

struct tPlatformColliderInfo
{
	PLATFORM_TYPE	Type;
	int				Priority;	// 낮을 수록 우선순위

	// RAMP일 경우
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

// DebugFont 구조체
struct tDebugFont
{
	const wchar_t* szBuff;
	float PosX;
	float PosY;
	float FontSize;
	UINT  Color;
};

// DebugShape 구조체
struct tDebugShapeInfo
{
	SHAPE_TYPE  eShape;			// 모양
	Vec3		vWorldPos;		// 위치
	Vec3		vWorldScale;	// 크기
	Vec3		vWorldRot;		// 회전상태
	Matrix		matWorld;		// 위치.크기.회전이 매트릭스로 들어온 경우를 대비, 3요소로 들어온 경우 단위행렬
	Vec3		vStartPos;
	Vec3		vEndPos;
	Vec4		vColor;			// 색상
	float		fMaxTime;		// 노출할 시간
	float		fCurTime;		// 현재 시간
};


struct tLightColor
{
	Vec4 vDiffuse;	// 빛의 색상
	Vec4 vAmbient;	// 주변 광(환경 광)
};

// LightInfo
struct tLightInfo
{
	tLightColor Color;		 // 빛의 색상

	Vec4		vWorldPos;   // 광원의 월드 스페이스 위치
	Vec4		vWorldDir;	 // 빛을 보내는 방향

	UINT		LightType;   // 빛의 타입(방향성, 점, 스포트) -> 상수버퍼에 바인딩하기 위하여 enum이 아닌 UINT
	float		Radius;		 // 빛의 반경(사거리)
	float		Angle;		 // 빛의 각도
	float		Intensity;	 
};

// TileMap
struct tTile
{
	Vec2 vLeftTop;		// 타일 좌상단 
	Vec2 vSlice;		// 타일 크기
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
	Vec4	vLocalPos;		// 오브젝트로부터 떨어진 거리
	Vec4	vWorldPos;		// 파티클 위치
	Vec4	vWorldScale;	// 파티클 크기
	Vec4	vColor;			// 파티클 색상
	Vec4	vVelocity;		// 파티클 현재 속도
	Vec4	vForce;			// 파티클에 주어진 힘
	Vec4	vRandomForce;	// 파티클에 적용되는 랜덤방향 힘

	float   Age;			// 생존 시간
	float   PrevAge;		// 이전 프레임 생존 시간
	float   NomalizedAge;	// 수명대비 생존시간을 0~1로 정규화 한 값
	float	LifeTime;		// 수명
	float	Mass;			// 질량
	float   ScaleFactor;	// 추가 크기 배율

	int     Active;			// 파티클 활성화 여부
	int     pad;
};

struct tRWParticleBuffer
{
	int		SpawnCount;			// 스폰 시킬 파티클 개수
	int		padding[3];
};

struct tParticleModule
{
	// 스폰 모듈
	Vec4    vSpawnColor;
	Vec4	vSpawnScaleMin;
	Vec4	vSpawnScaleMax;
	Vec3	vBoxShapeScale;
	float	fSphereShapeRadius;
	int		SpawnShapeType;		// 0 : BOX, 1 : Sphere
	int		SpawnRate;			// 초당 생성 개수
	int		Space;				// 파티클 업데이트 좌표계 ( 0 : World,  1 : Local)
	float   MinLifeTime;		// 최소 수명
	float   MaxLifeTime;		// 최대 수명
	int     spawnpad[3];

	// Color Change 모듈
	Vec4	vStartColor;		// 초기 색상
	Vec4	vEndColor;			// 최종 색상

	// Scale Change 모듈
	float	StartScale;		// 초기 크기
	float	EndScale;			// 최종 크기	

	// 버퍼 최대크기
	int		iMaxParticleCount;
	int		ipad;

	// Add Velocity 모듈
	Vec4	vVelocityDir;
	int     AddVelocityType;	// 0 : From Center, 1: To Center, 2 : Fixed Direction	
	float	OffsetAngle;
	float	Speed;
	int     addvpad;

	// Drag 모듈 - 속도 제한
	float	StartDrag;
	float	EndDrag;

	// NoiseForce 모듈 - 랜덤 힘 적용	
	float	fNoiseTerm;		// 랜덤 힘 변경 간격
	float	fNoiseForce;	// 랜덤 힘 크기

	// Render 모듈
	int		VelocityAlignment;	// 1 : 속도정렬 사용(이동 방향으로 회전) 0 : 사용 안함
	int		VelocityScale;		// 1 : 속도에 따른 크기 변화 사용, 0 : 사용 안함	
	float   vMaxSpeed;			// 최대 크기에 도달하는 속력
	Vec4	vMaxVelocityScale;	// 속력에 따른 크기 변화량 최대치
	int		renderpad;

	// Module Check	(모듈을 사용할지 말지에 대한 여부)
	int		ModuleCheck[(UINT)PARTICLE_MODULE::END];
};


// =======================
// 상수 버퍼 대응 구조체
// =======================

struct tTransform		// 상수 버퍼를 통해 전달할 오브젝트의 행렬 정보
{
	Matrix matWorld;
	Matrix matView;
	Matrix matProj;

	Matrix matWV;		// World 행렬 * View 행렬
	Matrix matWVP;		// World 행렬 * View 행렬 * Projection 행렬
};

extern tTransform g_transform;


struct tMtrlConst		// 재질을 통해 전송할 상수값들 타입별로 4개씩 정의 + 텍스처 레지스터에 텍스처가 바인딩 되었는지 여부 8개
{
	int arrInt[4];
	float arrFloat[4];
	Vec2 arrV2[4];
	Vec4 arrV4[4];
	Matrix arrMat[4];

	int arrTex[(UINT)TEX_PARAM::TEX_END];
};

struct tGlobal			// 전역 상수 버퍼에서 사용하는 구조체
{
	Vec2  Resolution;
	float tDT;
	float tAccTime;
	UINT  Light2DCount;
	UINT  Light3DCount;
	int	  globalpadding[2];
};

extern tGlobal GlobalData; // 전역으로 사용할 수 있게 extern 선언