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
	event.wParam = (DWORD_PTR)	_NewObject;	 // �߰��� ������Ʈ�� �ּҰ�
	event.lParam = _LayerIdx;

	CEventManager::GetInst()->AddEvent(event);
}

void SpawnGameObject(CGameObject* _NewObject, Vec3 _vWorldPos, const wstring& _strName)
{
	_NewObject->Transform()->SetRelativePos(_vWorldPos);

	tEvent event = {};
 
	event.Type = EVENT_TYPE::CREATE_OBJECT;
	event.wParam = (DWORD_PTR)_NewObject;	 // �߰��� ������Ʈ�� �ּҰ�
	event.lParam = CLevelManager::GetInst()->GetCurLevel()->FindLayerByName(_strName)->GetLayerIndex();

	CEventManager::GetInst()->AddEvent(event);
}

void DestroyObject(CGameObject* _DeletObject)
{
	// �ߺ���üó���� ���� ���� �̹� Dead������ ������Ʈ�� ���Ͻ�Ų��.
	if (_DeletObject->IsDead())
		return;

	tEvent event = {};

	event.Type = EVENT_TYPE::DELETE_OBJECT;		
	event.wParam = (DWORD_PTR)_DeletObject;		// ������ ������Ʈ�� �ּҰ�

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
	// ���۸� 0���� �ʱ�ȭ
	memset(_Buff, 0, sizeof(char) * _BufferSize);

	string str = string(_wstr.begin(), _wstr.end());

	// ���ۿ� 1����Ʈ ���ڿ��� ��ȯ�� ���ڿ��� memcpy
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
	// Load �� �������� ������ ���ڿ����� �ľ��ϱ� ���� ���ڿ��� ���� ����
	UINT iLen = (UINT)_str.length();
	fwrite(&iLen, sizeof(UINT), 1, _File);

	// ���� ���ڿ� ����
	fwrite(_str.c_str(), sizeof(wchar_t), _str.length(), _File);
}

void LoadWString(wstring& _str, FILE* _File)
{
	// �ҷ��� ���� ����
	wchar_t szBuffer[256] = {};
	
	// ���ڿ��� ���� �ľ� �� �׸�ŭ �ҷ���
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

	// �����ϴ� ���ҽ��� ���� �� i�� 0 ����
	if (nullptr == _Res)
	{
		fwrite(&i, sizeof(i), 1, _File);
	}
	// �����ϴ� ���ҽ��� ���� �� i�� 1 ����, Ű�� + ��ΰ��� ����
	else
	{
		i = 1;
		fwrite(&i, sizeof(i), 1, _File);
		SaveWString(_Res->GetKey(), _File);
		SaveWString(_Res->GetRelativePath(), _File);
	}
}

