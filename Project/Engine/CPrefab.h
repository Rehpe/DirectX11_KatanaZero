#pragma once
#include "CResource.h"

class CGameObject;

class CPrefab :
    public CResource
{
public:
    CPrefab();
    ~CPrefab();

public:
    CLONE(CPrefab);

private:
    CGameObject* m_ProtoObj;        // 프리팹은 멤버로 복사할 게임 오브젝트를 가진다

public:
    CGameObject* Instantiate();
    void AddToLayer(int _iLayerIdx);
    void AddToLayer(wstring _LayerName);


private:
    virtual int Load(const wstring& _strFilePath);
public:
    virtual int Save(const wstring& _strRelativePath);

private:
    virtual void UpdateData() override {};

public:
    void RegisterProtoObject(CGameObject* _Proto);  // 원본 오브젝트를 프리팹으로 등록하는 함수


};

