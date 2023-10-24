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
	TRANSFORM,			// ��ġ, ũ��, ȸ��
	COLLIDER2D,			// 2���� �浹
	COLLIDER3D,			// 3���� �浹
	ANIMATOR2D,			// Sprite Animation
	ANIMATOR3D,			// Bone Skinning Animation
	LIGHT2D,			// 2���� ����
	LIGHT3D,			// 3���� ����
	CAMERA,				// ī�޶�
	RIGIDBODY2D,		// ����
	FSM_AI,				// FSM

	// render
	MESHRENDER,			// �⺻���� ������
	PARTICLESYSTEM,		// ���� ������
	TILEMAP,			// 2���� Ÿ��
	LANDSCAPE,			// 3���� ����
	DECAL,				// ���� ������

	END,

	// custom
	SCRIPT,				// �������� �������� �ʴ� ������� ���� ������Ʈ, ������Ʈ�̱� �ϳ� ��ũ��Ʈ ������Ʈ��
						// ��ũ��Ʈ ���ͷ� ���� �����ϱ� ������ End ���ķ� �����Ͽ� Component_type ��ȸ���� �����Ѵ�. 
};

extern const char* COMPONENT_TYPE_STR[(UINT)COMPONENT_TYPE::END];
extern const wchar_t* COMPONENT_TYPE_WSTR[(UINT)COMPONENT_TYPE::END];

enum class RES_TYPE
{
	MESHDATA,
	MATERIAL,
	PREFAB,

	MESH,			// ����
	TEXTURE,		// �̹���
	SOUND,

	GRAPHICS_SHADER,
	COMPUTE_SHADER,

	END,
};

extern const char* RES_TYPE_STR[(UINT)RES_TYPE::END];
extern const wchar_t* RES_TYPE_WSTR[(UINT)RES_TYPE::END];

enum class CB_TYPE			
{
	// ��� ���� Ÿ�� �������Ӱ� ���ÿ� �ش� ��������� �������� ��ȣ


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
	CULL_BACK,			// ������ ������ �ݽð������ �޽��� �׸��� ����
	CULL_FRONT,			// ���� �ݴ�� ������ ������ �ð������ �޽��� �׸��� ����(�÷��̾ ���θ��� ���ԵǴ� �� ����� Skybox � Ȱ��ȴ�)
	CULL_NONE,			// ��� �����̶� ���� �� �׸� (2D���� ����Ʈ �ɼ�)
	WIRE_FRAME,			// ����(��)�� ä���� �ʰ�, ������ �մ� ���� ����� (Topology �ɼ��� Line �ɼǰ� ���)
	END,
};

enum class DS_TYPE
{
	LESS,				// ������ ��ϵ� ���̺��� �� ������� �����. (�⺻)
	LESS_EQUAL,			// ������ ��ϵ� ���̺��� �����ų� ������ �����.
	GREATER,			// ������ ��ϵ� ���̺��� �� �־�� �����
	GREATER_EQUAL,		// ������ ��ϵ� ���̺��� �ְų� ������ �����.
	NO_WRITE,			// ���� ������ LESS �������� �ϵ� ��µ� ������Ʈ�� ���̴� ������� ����
	NO_TEST_NO_WRITE,	// ���� ������ ���� �ʰ� ���̵� ������� �ʰ� ������ �����. (UI ��)
	END,
};

enum class BS_TYPE
{
	DEFAULT,			// ������ ���� �ʰ� ������ ������ �״�� ���
	MASK,				// Alpha Coverage
	ALPHA_BLEND,		// ���İ��� ��������� Ȱ���Ͽ� ������ ������ ���
	ONE_ONE,			// 1��1 ȥ�� (�׷��� ������Ʈ�� �÷�������, ���� ���� ������Ʈ�� �÷������� 1�� ����) : ���� ��� ����Ʈ�� ������ �� �� ��� (�������� RGB�� 000�̹Ƿ� 1�� ���ص� 0�̱� ������ �����ϰ� ��µ�)
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
	ORTHOGRAPHIC,		// ������ ���� ��������(����� ȭ���ػ󵵰� 1��1 ��Ī)
	PERSPECTIVE,		// ���� ����
};

enum class SHADER_DOMAIN		//���̴� ����, ���� �����ڸ� ������ ������ �����ϴ� ���̴��� Ư��(Pipeline�� DS �ܰ�� �ٸ�)
{
	DOMAIN_OPAQUE,			// ���� ��ü ������(3D�� ����)
	DOMAIN_MASK,			// ������ �ǻ�ü + ���� ���� ���� (2D���� ���� ���� ���, AlphatoCoverage �ɼ� Ȥ�� clip/discard ���)
	DOMAIN_TRANSPARENT,		// ������ (�� ������ ���, ������ ������ ������ ���� �����̾����)
	DOMAIN_POSTPROCESS,		// ��ó��

	DOMAIN_UI,				// ���� ���Ŀ� �׷����� UI
	DOMAIN_UNDEFINED,		// ������ ���� ����
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


// DebugShape ��� Ȱ��� ���� ����
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
	DIRECTIONAL,		// ���⼺ ����
	POINT,				// ����Ʈ ����
	SPOT,				// ���� ����
};

enum class LEVEL_STATE
{
	PLAY,
	PAUSE,
	STOP,
};

// ����ȭ ���� ���� : �б�����(t��������), �б�/����(u��������)
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
