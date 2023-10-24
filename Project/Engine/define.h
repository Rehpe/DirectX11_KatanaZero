#pragma once


#define DEVICE	CDevice::GetInst()->GetDevice()
#define CONTEXT CDevice::GetInst()->GetDeviceContext()

#define CLONE(type) virtual type* Clone() { return new type(*this);}
#define CLONE_DEACTIVATE(type) virtual type* Clone() {return nullptr; assert(nullptr);}

#define KEY_TAP(Key) CKeyManager::GetInst()->GetKeyState(Key) == KEY_STATE::TAP
#define KEY_PRESSED(Key) CKeyManager::GetInst()->GetKeyState(Key) == KEY_STATE::PRESSED
#define KEY_RELEASED(Key) CKeyManager::GetInst()->GetKeyState(Key) == KEY_STATE::RELEASE

#define DT CTimeManager::GetInst()->GetDeltaTime()

#define MAX_LAYER 32

#define SINGLETON(type) private: type(); ~type(); friend class CSingleton<type>;


enum class COMPONENT_TYPE
{
	// update
	TRANSFORM,			// 위치, 크기, 회전
	COLLIDER2D,			// 2차원 충돌
	COLLIDER3D,			// 3차원 충돌
	ANIMATOR2D,			// Sprite Animation
	ANIMATOR3D,			// Bone Skinning Animation
	LIGHT2D,			// 2차원 광원
	LIGHT3D,			// 3차원 광원
	CAMERA,				// 카메라
	RIGIDBODY2D,		// 물리
	FSM_AI,				// FSM

	// render
	MESHRENDER,			// 기본적인 렌더링
	PARTICLESYSTEM,		// 입자 렌더링
	TILEMAP,			// 2차원 타일
	LANDSCAPE,			// 3차원 지형
	DECAL,				// 내부 렌더링

	END,

	// custom
	SCRIPT,				// 엔진에서 제공하지 않는 사용자의 구현 컴포넌트, 컴포넌트이긴 하나 스크립트 컴포넌트는
						// 스크립트 벡터로 따로 관리하기 때문에 End 이후로 선언하여 Component_type 순회에서 제외한다. 
};

extern const char* COMPONENT_TYPE_STR[(UINT)COMPONENT_TYPE::END];
extern const wchar_t* COMPONENT_TYPE_WSTR[(UINT)COMPONENT_TYPE::END];

enum class RES_TYPE
{
	MESHDATA,
	MATERIAL,
	PREFAB,

	MESH,			// 형태
	TEXTURE,		// 이미지
	SOUND,

	GRAPHICS_SHADER,
	COMPUTE_SHADER,

	END,
};

extern const char* RES_TYPE_STR[(UINT)RES_TYPE::END];
extern const wchar_t* RES_TYPE_WSTR[(UINT)RES_TYPE::END];

enum class CB_TYPE			
{
	// 상수 버퍼 타입 열거형임과 동시에 해당 상수버퍼의 레지스터 번호


	TRANSFORM,		// b0
	MATERIAL,		// b1
	GLOBAL,			// b2
	
	END,

};

enum SCALAR_PARAM
{
	INT_0,
	INT_1,
	INT_2,
	INT_3,

	FLOAT_0,
	FLOAT_1,
	FLOAT_2,
	FLOAT_3,

	VEC2_0,
	VEC2_1,
	VEC2_2,
	VEC2_3,

	VEC4_0,
	VEC4_1,
	VEC4_2,
	VEC4_3,

	MAT_0,
	MAT_1,
	MAT_2,
	MAT_3,
};

enum TEX_PARAM
{
	TEX_0,
	TEX_1,
	TEX_2,
	TEX_3,
	TEX_4,
	TEX_5,
	TEX_6,
	TEX_7,

	TEX_END,
};

enum PIPELINE_STAGE
{
	PS_VERTEX = 0x01,
	PS_HULL = 0x02,
	PS_DOMAIN = 0x04,
	PS_GEOMETRY = 0x08,
	PS_PIXEL = 0x10,

	PS_ALL = PS_VERTEX | PS_HULL | PS_DOMAIN | PS_GEOMETRY | PS_PIXEL,

};

enum class RS_TYPE
{
	CULL_BACK,			// 정점의 순서가 반시계방향인 메쉬는 그리지 않음
	CULL_FRONT,			// 위와 반대로 정점의 순서가 시계방향인 메쉬는 그리지 않음(플레이어가 내부만을 보게되는 구 모양의 Skybox 등에 활용된다)
	CULL_NONE,			// 어느 방향이라도 전부 다 그림 (2D게임 디폴트 옵션)
	WIRE_FRAME,			// 내부(면)를 채우지 않고, 정점을 잇는 선만 출력함 (Topology 옵션의 Line 옵션과 비슷)
	END,
};

enum class DS_TYPE
{
	LESS,				// 이전에 기록된 깊이보다 더 가까워야 출력함. (기본)
	LESS_EQUAL,			// 이전에 기록된 깊이보다 가깝거나 같으면 출력함.
	GREATER,			// 이전에 기록된 깊이보다 더 멀어야 출력함
	GREATER_EQUAL,		// 이전에 기록된 깊이보다 멀거나 같으면 출력함.
	NO_WRITE,			// 깊이 판정은 LESS 설정으로 하되 출력될 오브젝트의 깊이는 기록하지 않음
	NO_TEST_NO_WRITE,	// 깊이 판정도 하지 않고 깊이도 기록하지 않고 무조건 출력함. (UI 등)
	END,
};

enum class BS_TYPE
{
	DEFAULT,			// 블렌딩을 하지 않고 순수한 색깔을 그대로 출력
	MASK,				// Alpha Coverage
	ALPHA_BLEND,		// 알파값을 계수값으로 활용하여 투명도를 가지게 출력
	ONE_ONE,			// 1대1 혼합 (그려질 오브젝트의 컬러값에도, 기존 덮힌 오브젝트의 컬러값에도 1을 곱함) : 검은 배경 이펙트에 검은생 뺄 때 사용 (검은색은 RGB가 000이므로 1을 곱해도 0이기 때문에 투명하게 출력됨)
	END,
};

enum class DIR_TYPE
{
	RIGHT,
	UP,
	FRONT,
};

enum class PROJ_TYPE
{
	ORTHOGRAPHIC,		// 원근이 없는 직교투영(월드와 화면해상도가 1대1 매칭)
	PERSPECTIVE,		// 원근 투영
};

enum class SHADER_DOMAIN		//셰이더 영역, 쉽게 말하자면 렌더링 순서에 관여하는 셰이더의 특성(Pipeline의 DS 단계와 다름)
{
	DOMAIN_OPAQUE,			// 영역 전체 불투명(3D에 적합)
	DOMAIN_MASK,			// 불투명 피사체 + 완전 투명 여백 (2D에서 보통 자주 사용, AlphatoCoverage 옵션 혹은 clip/discard 사용)
	DOMAIN_TRANSPARENT,		// 반투명 (이 설정인 경우, 렌더링 순서가 무조건 가장 나중이어야함)
	DOMAIN_POSTPROCESS,		// 후처리

	DOMAIN_UI,				// 가장 최후에 그려지는 UI
	DOMAIN_UNDEFINED,		// 도메인 설정 미정
};

enum class EVENT_TYPE
{
	CREATE_OBJECT,	// wParam : GameObject, lParam : Layer Index
	DELETE_OBJECT,  // wParam : GameObject

	ADD_CHILD,
	CHANGE_LAYER,	// wParam : GameObject, lParam : Layer Index
	ADD_PREFAB,
	DELETE_RESOURCE,

	LEVEL_CHANGE,
};


// DebugShape 등에서 활용될 도형 종류
enum class SHAPE_TYPE
{
	RECT,
	CIRCLE,
	LINE,

	CUBE,
	SPHERE,

	END,
};

enum class COLLIDER2D_TYPE
{
	RECT,
	CIRCLE,
	LINE
};

enum class LIGHT_TYPE
{
	DIRECTIONAL,		// 방향성 광원
	POINT,				// 포인트 광원
	SPOT,				// 스팟 광원
};

enum class LEVEL_STATE
{
	PLAY,
	PAUSE,
	STOP,
};

// 구조화 버퍼 종류 : 읽기전용(t레지스터), 읽기/쓰기(u레지스터)
enum class SB_TYPE
{
	READ_ONLY,
	READ_WRITE,
};

enum class PARTICLE_MODULE
{
	PARTICLE_SPAWN,
	COLOR_CHANGE,
	SCALE_CHANGE,
	ADD_VELOCITY,

	DRAG,
	NOISE_FORCE,
	RENDER,
	DUMMY_3,

	END,
};

enum class PLATFORM_TYPE
{
	NONE,
	GROUND,
	RAMP,
	ONEWAY_PLATFORM,
	BREAKABLE_FLOOR,
};

enum class WALL_TYPE
{
	NONE,
	BLOCKWALL_LEFT,
	BLOCKWALL_RIGHT,
	BLOCKWALL_TOP,
	HANGABLEWALL_LEFT,
	HANGABLEWALL_RIGHT,

	END,
};


enum class EffectType
{
	DUSTCLOUD,
	JUMPCLOUD,
	LANDCLOUD,
	BLOOD,
	BULLETREFLECT,
	SLASH,
	LONGSLASH,
};
