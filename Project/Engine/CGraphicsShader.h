#pragma once
#include "CShader.h"

// 렌더링 작업용의 Shader. 렌더링 파이프라인 전체가 이 한 클래스 안에 들어있다고 생각하면 된다

// 공개할 상수 / 텍스처 파라미터들을 표현할 구조체
struct tScalarParam
{
    SCALAR_PARAM    eParam;
    string          strDesc;
};

struct tTexParam
{
    TEX_PARAM       eParam;
    string          strDesc;
};


class CGraphicsShader :
    public CShader
{
public:
    CGraphicsShader();
    ~CGraphicsShader();

private:
    ComPtr<ID3DBlob>                m_VSBlob;
    ComPtr<ID3DBlob>                m_HSBlob;
    ComPtr<ID3DBlob>                m_DSBlob;
    ComPtr<ID3DBlob>                m_GSBlob;
    ComPtr<ID3DBlob>                m_PSBlob;

    ComPtr<ID3D11VertexShader>      m_VS;
    ComPtr<ID3D11HullShader>        m_HS;
    ComPtr<ID3D11DomainShader>      m_DS;
    ComPtr<ID3D11GeometryShader>    m_GS;
    ComPtr<ID3D11PixelShader>       m_PS;

    ComPtr<ID3D11InputLayout>	    m_Layout;
    D3D11_PRIMITIVE_TOPOLOGY        m_eTopology;

    RS_TYPE                         m_RSType;

    DS_TYPE                         m_DSType;

    BS_TYPE                         m_BSType;

    SHADER_DOMAIN                   m_Domain;

    // 외부에 공개할 파라미터들을 담은 벡터
    vector<tScalarParam>            m_vecScalarParam;
    vector<tTexParam>               m_vecTexParam;
    

public:
    void CreateVertexShader(const wstring& _strFileName, const string& _strFuncName);
    void CreateGeometryShader(const wstring& _strFileName, const string& _strFuncName);
    void CreatePixelShader(const wstring& _strFileName, const string& _strFuncName);
    void SetTopology(D3D11_PRIMITIVE_TOPOLOGY _Topology) { m_eTopology = _Topology; }
    void SetRSType(RS_TYPE _type) { m_RSType = _type; }
    void SetDSType(DS_TYPE _type) { m_DSType = _type; }
    void SetBSType(BS_TYPE _type) { m_BSType = _type; }
    void SetDomain(SHADER_DOMAIN _domain) { m_Domain = _domain; }
    SHADER_DOMAIN GetDomain() { return m_Domain; }

    virtual void UpdateData() override;

    void AddScalarParam(SCALAR_PARAM _Param, const string& _desc) { m_vecScalarParam.push_back(tScalarParam{ _Param, _desc }); }
    void AddTexParam(TEX_PARAM _Param, const string& _desc) { m_vecTexParam.push_back(tTexParam{ _Param, _desc }); }

    const vector<tScalarParam>& GetScalarParam() { return m_vecScalarParam; }
    const vector<tTexParam>& GetTexParam() { return m_vecTexParam; }
};

