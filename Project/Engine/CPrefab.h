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
    CGameObject* m_ProtoObj;        // �������� ����� ������ ���� ������Ʈ�� ������

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
    void RegisterProtoObject(CGameObject* _Proto);  // ���� ������Ʈ�� ���������� ����ϴ� �Լ�


};

