#include "pch.h"

#include "CEventManager.h"
#include "CLevelManager.h"
#include "CLevel.h"
#include "CLayer.h"
#include "CGameObject.h"
#include "CTransform.h"
#include "CRenderManager.h"
#include "ptr.h"
#include "CResource.h"

void SpawnGameObject(CGameObject* _NewObject, Vec3 _vWorldPos, int _LayerIdx)
{
	_NewObject->Transform()->SetRelativePos(_vWorldPos);
	
	tEvent event = {};

	event.Type = EVENT_TYPE::CREATE_OBJECT;
	event.wParam = (DWORD_PTR)	_NewObject;	 // 추가할 오브젝트의 주소값
	event.lParam = _LayerIdx;

	CEventManager::GetInst()->AddEvent(event);
}

void SpawnGameObject(CGameObject* _NewObject, Vec3 _vWorldPos, const wstring& _strName)
{
	_NewObject->Transform()->SetRelativePos(_vWorldPos);

	tEvent event = {};
 
	event.Type = EVENT_TYPE::CREATE_OBJECT;
	event.wParam = (DWORD_PTR)_NewObject;	 // 추가할 오브젝트의 주소값
	event.lParam = CLevelManager::GetInst()->GetCurLevel()->FindLayerByName(_strName)->GetLayerIndex();

	CEventManager::GetInst()->AddEvent(event);
}

void DestroyObject(CGameObject* _DeletObject)
{
	// 중복삭체처리를 막기 위해 이미 Dead상태인 오브젝트는 리턴시킨다.
	if (_DeletObject->IsDead())
		return;

	tEvent event = {};

	event.Type = EVENT_TYPE::DELETE_OBJECT;		
	event.wParam = (DWORD_PTR)_DeletObject;		// 삭제할 오브젝트의 주소값

	CEventManager::GetInst()->AddEvent(event);
}

void DrawDebugRect(Vec3 _vWorldPos, Vec2 _vWorldScale, Vec4 _vColor, Vec3 _vRotation, float _fTime)
{
	tDebugShapeInfo info = {};

	info.matWorld = XMMatrixIdentity();
	info.eShape = SHAPE_TYPE::RECT;
	info.fMaxTime = _fTime;
	info.vWorldPos = _vWorldPos;
	info.vWorldScale = Vec3(_vWorldScale.x, _vWorldScale.y, 1.f);
	info.vWorldRot = _vRotation;
	info.vColor = _vColor;

	CRenderManager::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugRect(const Matrix& _matWorld, Vec4 _vColor, float _fTime)
{
	tDebugShapeInfo info = {};

	info.matWorld = _matWorld;
	info.eShape = SHAPE_TYPE::RECT;
	info.fMaxTime = _fTime;
	info.vColor = _vColor;

	CRenderManager::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugCircle(Vec3 _vWorldPos, float _fRadius, Vec4 _vColor, Vec3 _vRotation, float _fTime)
{
	tDebugShapeInfo info = {};

	info.matWorld = XMMatrixIdentity();
	info.eShape = SHAPE_TYPE::CIRCLE;
	info.fMaxTime = _fTime;
	info.vWorldPos = _vWorldPos;
	info.vWorldScale = Vec3(_fRadius, _fRadius, 1.f);
	info.vWorldRot = _vRotation;
	info.vColor = _vColor;

	CRenderManager::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugCircle(const Matrix& _matWorld, Vec4 _vColor, float _fTime)
{
	tDebugShapeInfo info = {};

	info.matWorld = _matWorld;
	info.eShape = SHAPE_TYPE::CIRCLE;
	info.fMaxTime = _fTime;
	info.vColor = _vColor;

	CRenderManager::GetInst()->AddDebugShapeInfo(info);
}

void DrawDebugLine(Vec3 _vStartPos, Vec3 _vEndPos, Vec4 _vColor, float _fTime)
{
	tDebugShapeInfo info = {};

	info.eShape = SHAPE_TYPE::LINE;
	info.fMaxTime = _fTime;
	info.vStartPos = _vStartPos;
	info.vEndPos = _vEndPos;
	info.vWorldRot = Vec3(0.f, 0.f, 0.f); // Assuming no rotation for the line
	info.vColor = _vColor;

	CRenderManager::GetInst()->AddDebugShapeInfo(info);
}


wstring stringToWstring(string& str)
{
	//std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
	//return converter.from_bytes(str);

	return wstring().assign(str.begin(), str.end());
}

void WstringToChar(wstring _wstr, char* _Buff, size_t _BufferSize)
{
	// 버퍼를 0으로 초기화
	memset(_Buff, 0, sizeof(char) * _BufferSize);

	string str = string(_wstr.begin(), _wstr.end());

	// 버퍼에 1바이트 문자열로 변환한 문자열을 memcpy
	memcpy(_Buff, str.data(), sizeof(char) * str.length());
}

const char* ToString(COMPONENT_TYPE type)
{
	return COMPONENT_TYPE_STR[(UINT)type];
}

const wchar_t* ToWString(COMPONENT_TYPE type)
{
	return COMPONENT_TYPE_WSTR[(UINT)type];
}

const char* ToString(RES_TYPE type)
{
	return RES_TYPE_STR[(UINT)type];
}

const wchar_t* ToWSTring(RES_TYPE type)
{
	return RES_TYPE_WSTR[(UINT)type];
}

void SaveWString(const wstring& _str, FILE* _File)
{
	// Load 시 어디까지가 지정한 문자열인지 파악하기 위해 문자열의 길이 저장
	UINT iLen = (UINT)_str.length();
	fwrite(&iLen, sizeof(UINT), 1, _File);

	// 실제 문자열 저장
	fwrite(_str.c_str(), sizeof(wchar_t), _str.length(), _File);
}

void LoadWString(wstring& _str, FILE* _File)
{
	// 불러올 버퍼 생성
	wchar_t szBuffer[256] = {};
	
	// 문자열의 길이 파악 후 그만큼 불러옴
	UINT iLen = 0;
	fread(&iLen, sizeof(UINT), 1, _File);
	fread(szBuffer, sizeof(wchar_t), iLen, _File);

	_str = szBuffer;
}

float Lerp(float a, float b, float t)
{
	return a + (b - a) * t;
}

void SaveResourceRef(Ptr<CResource> _Res, FILE* _File)
{
	int i = 0;

	// 참조하는 리소스가 없을 시 i에 0 저장
	if (nullptr == _Res)
	{
		fwrite(&i, sizeof(i), 1, _File);
	}
	// 참조하는 리소스가 있을 시 i에 1 저장, 키값 + 경로값도 저장
	else
	{
		i = 1;
		fwrite(&i, sizeof(i), 1, _File);
		SaveWString(_Res->GetKey(), _File);
		SaveWString(_Res->GetRelativePath(), _File);
	}
}

