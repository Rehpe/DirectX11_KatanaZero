#pragma once
#include "CEntity.h"

class CResource :
    public CEntity
{
public:
    CResource(RES_TYPE _type, bool _bEngine = false);
    CResource(const CResource& _Other);
    virtual ~CResource();

    friend class CResourceManager;

    template<typename T>
    friend class Ptr;               // ���� �������� �߰�/���Ҹ� ���� Ptr Ŭ������ friend ó��

public:
    // ���ҽ��� Clone �� �������� �ʴ´�.
    virtual CResource* Clone() { return nullptr; assert(nullptr); }

    // ���ҽ� ���ε� �Լ�, ���������Լ��� ��� ���ҽ��� ��� ���ε� �Լ��� �����ؾ��Ѵ�.
    virtual void UpdateData() = 0;

private:
    const RES_TYPE  m_Type;
    int             m_iRefCount;

    wstring         m_strKey;
    wstring         m_strRelativePath;

    bool            m_bEngine;      // �������� ������� ���ҽ��ΰ�?

private:
    // ���� ��ηκ��� �ε� (������) ----> ���ҽ� �Ŵ����� ���ؼ��� ȣ�� ����
    virtual int Load(const wstring& _strFilePath) = 0;

public:
    // ���Ϸ� ���� (�����) ----> ���ҽ� �Ŵ��� ���̵� ȣ�� ����, ���� public
    virtual int Save(const wstring& _strRelativePath) = 0;

private:
    void SetKey(const wstring& _strKey) { m_strKey = _strKey; }
    void SetRelativePath(const wstring& _strPath) { m_strRelativePath = _strPath; }

    void AddRef() { ++m_iRefCount; }
    void Release();

public:
    const wstring& GetKey() { return m_strKey; }
    const wstring& GetRelativePath() { return m_strRelativePath; }
    RES_TYPE GetType() { return m_Type; }
    bool IsEngineRes() { return m_bEngine; }
};

