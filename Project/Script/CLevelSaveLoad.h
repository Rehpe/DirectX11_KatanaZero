#pragma once

// 생성/소멸자, 멤버변수 없이 오로지 정적멤버함수만 가지는 클래스

class CLevel;
class CLayer;
class CGameObject;
class CPrefab;

class CLevelSaveLoad
{
public:
	static int SaveLevel(const wstring& _LevelPath, CLevel* _Level);
	static int SaveGameObject(CGameObject* _Object, FILE* _File);
	static int SavePrefab(const wstring& _prefabPath, CGameObject* _protoObj);

	static CLevel* LoadLevel(const wstring& _LevelPath);
	static CGameObject* LoadGameObject(FILE* _File);
	static Ptr<CPrefab> LoadPrefab(const wstring& _prefabPath);


};

