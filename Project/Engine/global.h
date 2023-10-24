#pragma once

#include <vector>
#include <list>
#include <map>
#include <string>

using std::vector;
using std::list;
using std::map;
using std::make_pair;
using std::wstring;
using std::string;

#include <typeinfo>
#include <assert.h>


// FileSystem(����� ���)
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
using namespace std::experimental;
using namespace std::experimental::filesystem;


#include <wrl.h>			//����Ʈ ������ ���� ���
using namespace Microsoft::WRL;


#include <d3d11.h>
#include <d3dcompiler.h>	//Shader �����Ϸ�
#include <DirectXMath.h>	//���� ���� �Լ�
#include <DirectXPackedVector.h>

using namespace DirectX;
using namespace DirectX::PackedVector;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")


#include "SimpleMath.h"
using namespace DirectX::SimpleMath;

typedef Vector2 Vec2;
typedef Vector3	Vec3;
typedef Vector4 Vec4;

#include "define.h"
#include "struct.h"
#include "function.h"

#include "CSingleton.h"
