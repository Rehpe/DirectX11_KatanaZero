#pragma once

// ����/�Ҹ���, ������� ���� ������ ��������Լ��� ������ Ŭ����

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

