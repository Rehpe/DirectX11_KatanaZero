#pragma once
#include <algorithm>

// 오브젝트 생성용 전역 함수
class CGameObject;

void SpawnGameObject(CGameObject* _NewObject, Vec3 _vWorldPos, int _LayerIdx);
void SpawnGameObject(CGameObject* _NewObject, Vec3 _vWorldPos, const wstring& _strName);

// 오브젝트 삭제용 전역 함수
void DestroyObject(CGameObject* _DeletObject);

// DebugShape 구조체를 보다 편하게 채우는 함수
void DrawDebugRect(Vec3 _vWorldPos, Vec2 _vWorldScale, Vec4 _vColor, Vec3 _vRotation, float _fTime = 0.f);
void DrawDebugRect(const Matrix& _matWorld, Vec4 _vColor, float _fTime = 0.f);


void DrawDebugCircle(Vec3 _vWorldPos, float _fRadius, Vec4 _vColor, Vec3 _vRotation, float _fTime = 0.f);
void DrawDebugCircle(const Matrix& _matWorld, Vec4 _vColor, float _fTime = 0.f);

void DrawDebugLine(Vec3 _vStartPos, Vec3 _vEndPos, Vec4 _vColor, float _fTime);

// void DrawDebugSphere();
// void DrawDebugCube();

wstring stringToWstring(string& str);
void WstringToChar(wstring _wstr, char* _Buff, size_t _BufferSize);

const char* ToString(COMPONENT_TYPE);
const wchar_t* ToWSTring(COMPONENT_TYPE);

const char* ToString(RES_TYPE);
const wchar_t* ToWSTring(RES_TYPE);

// Save / Load
void SaveWString(const wstring& _str, FILE* _File);
void LoadWString(wstring& _str, FILE* _File);

float Lerp(float a, float b, float t);

class CResource;
template<typename T>
class Ptr;

void SaveResourceRef(Ptr<CResource> _Res, FILE* _File);

class CResourceManager;
template<typename T>
void LoadResourceRef(Ptr<T>& _Res, FILE* _File)
{
	int i = 0;
	fread(&i, sizeof(i), 1, _File);

	// 참조하던 리소스가 있을 경우에 해당 리소스를 로드해온다.
	if (i)
	{
		wstring strKey, strRelativePath;
		LoadWString(strKey, _File);
		LoadWString(strRelativePath, _File);

		_Res = CResourceManager::GetInst()->Load<T>(strKey, strRelativePath);
	}
}





template<typename T, UINT Size>
void Safe_Del_Array(T* (&arr)[Size])
{
	for (UINT i = 0; i < Size; ++i)
	{
		if (nullptr != arr[i])
			delete arr[i];
	}
}

template<typename T>
void Safe_Del_Vec(vector<T*> _vec)
{
	for (size_t i = 0; i < _vec.size(); ++i)
	{
		if (nullptr != _vec[i])
			delete _vec[i];
	}
	_vec.clear();
}

template<typename T1, typename T2>
void Safe_Del_Map(map<T1, T2>& _map)
{
	for (const auto& pair : _map)
	{
		if (nullptr != pair.second)
			delete pair.second;
	}

	_map.clear();
}

template<typename T>
T lerp(const T& a, const T& b, float t)
{
	return (1 - t) * a + t * b;
}

template<typename T>
T clamp(const T& value, const T& Min, const T& Max)
{
	if (value < Min)
		return Min;
	else if (value > Max)
		return Max;
	else
		return value;
}

