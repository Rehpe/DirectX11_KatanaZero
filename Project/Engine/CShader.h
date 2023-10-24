#pragma once
#include "CResource.h"

class CShader :
    public CResource
{
public:
    CShader(RES_TYPE _eType);
    virtual ~CShader();

protected:
    ComPtr<ID3DBlob>      m_ErrBlob;


    // Shader의 경우 Load/Save가 불필요함으로 더미로 구현
public:
    virtual int Save(const wstring& _strRelativePath) { return S_OK; }
private:
    virtual int Load(const wstring& _strFilePath) { return S_OK; }

};

