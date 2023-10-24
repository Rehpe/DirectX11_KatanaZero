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
    friend class Ptr;               // 참조 포인터의 추가/감소를 위해 Ptr 클래스도 friend 처리

public:
    // 리소스는 Clone 을 구현하지 않는다.
    virtual CResource* Clone() { return nullptr; assert(nullptr); }

    // 리소스 바인딩 함수, 순수가상함수로 모든 리소스는 모두 바인딩 함수를 구현해야한다.
    virtual void UpdateData() = 0;

private:
    const RES_TYPE  m_Type;
    int             m_iRefCount;

    wstring         m_strKey;
    wstring         m_strRelativePath;

    bool            m_bEngine;      // 엔진에서 만들어진 리소스인가?

private:
    // 파일 경로로부터 로딩 (절대경로) ----> 리소스 매니저를 통해서만 호출 가능
    virtual int Load(const wstring& _strFilePath) = 0;

public:
    // 파일로 저장 (상대경로) ----> 리소스 매니저 없이도 호출 가능, 따라서 public
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

