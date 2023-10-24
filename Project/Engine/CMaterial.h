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

    // ù��° ���ڷ� ������� ��� ���ڿ� ������ enum��, �ι�° ���ڷ� ������ �������� �ּҰ�
    void SetScalarParam(SCALAR_PARAM _Param, const void* _Src);

    // ���Ÿ���� �پ��ϹǷ�, void �����͸� Ȱ���� out�������� ����� �޾ƺ� �� �ְ� �Լ� �ۼ�
    void GetScalarParam(SCALAR_PARAM _param, void* _pData);

    // ù ��° ���ڷ� �ؽ�ó ���� ��� ���ڿ� ���� �� enum��, �� ��° ���ڷ� ������ �ؽ�óPtr�� �ּҰ�
    void SetTexParam(TEX_PARAM _Param, const Ptr<CTexture>& _Tex);
    Ptr<CTexture> GetTexParam(TEX_PARAM _param) { return m_arrTex[(UINT)_param]; }

public:
    virtual void UpdateData() override;      // �������(m_Const)�� ������۷� �ű�� b1 �������Ϳ� ���ε�

private:
    virtual int Load(const wstring& _strFilePath);
public:
    virtual int Save(const wstring& _strRelativePath);
};

