#pragma once
#include "CResource.h"

#include "ptr.h"
#include "CGraphicsShader.h"
#include "CTexture.h"

class CMaterial :
    public CResource
{
public:
    CMaterial(bool _bEngine = false);
    ~CMaterial();

public:
    CLONE(CMaterial);

private:
    tMtrlConst              m_Const;
    Ptr<CTexture>           m_arrTex[TEX_END];

    Ptr<CGraphicsShader>    m_pShader;

     
public:
    void SetShader(Ptr<CGraphicsShader> _Shader) { m_pShader = _Shader; }
    Ptr<CGraphicsShader> GetShader() { return m_pShader; }

    // 첫번째 인자로 상수버퍼 어느 인자에 넣을지 enum값, 두번째 인자로 전달할 데이터의 주소값
    void SetScalarParam(SCALAR_PARAM _Param, const void* _Src);

    // 상수타입이 다양하므로, void 포인터를 활용해 out형식으로 결과를 받아볼 수 있게 함수 작성
    void GetScalarParam(SCALAR_PARAM _param, void* _pData);

    // 첫 번째 인자로 텍스처 버퍼 어느 인자에 넣을 지 enum값, 두 번째 인자로 전달할 텍스처Ptr의 주소값
    void SetTexParam(TEX_PARAM _Param, const Ptr<CTexture>& _Tex);
    Ptr<CTexture> GetTexParam(TEX_PARAM _param) { return m_arrTex[(UINT)_param]; }

public:
    virtual void UpdateData() override;      // 재질상수(m_Const)를 상수버퍼로 옮기고 b1 레지스터에 바인딩

private:
    virtual int Load(const wstring& _strFilePath);
public:
    virtual int Save(const wstring& _strRelativePath);
};

