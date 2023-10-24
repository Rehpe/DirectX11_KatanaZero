#pragma once
#include "CResource.h"

#include <DirectXTex\DirectXTex.h>

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex//DirectXTex_debug")
#else
#pragma comment(lib, "DirectXTex//DirectXTex")
#endif

// �ؽ�ó�� Texture2D  �ϳ���, �� �ؽ�ó�� ������ �� �ִ� �پ��� �÷����� View�� �̷�����ִ�.

class CTexture :
    public CResource
{
public:
    CTexture(bool _bEngine = false);
    ~CTexture();

private:
    ComPtr<ID3D11Texture2D>                 m_Tex2D;
    ComPtr<ID3D11ShaderResourceView>        m_SRV;      // �������Ϳ� ���ε��� �� ���Ǵ� ��
    ComPtr<ID3D11RenderTargetView>	        m_RTV;      // ���� Ÿ�ٿ� �ؽ�ó�� ������ �� ���Ǵ� ��
    ComPtr<ID3D11DepthStencilView>	        m_DSV;      // �������ٽǿ� �ؽ�ó�� ������ �� ���Ǵ� ��
    ComPtr<ID3D11UnorderedAccessView>       m_UAV;      // �б�, ���Ⱑ ���ÿ� ������ �ؽ�ó�� ������ �� ���Ǵ� ��
    
    D3D11_TEXTURE2D_DESC                    m_Desc;

    ScratchImage                            m_Image;    // ������ġ�� �ؽ�ó�� �ý��� �޸𸮷� �������� ���� ���

    UINT                                    m_iRecentNum;     // ���� �ֱ� ���ε��� CS �������� ��ȣ
public:
    int Create(UINT _Width, UINT _Height, DXGI_FORMAT _pixelformat
        , UINT _BindFlag, D3D11_USAGE _Usage);                      // ���ο� �ؽ�ó�� ������ ���

    int Create(ComPtr<ID3D11Texture2D> _tex2D);                     // Texture2D�� ������ ������ ���(Ex. Device�� RT)

public:
    virtual int Save(const wstring& _strRelativePath) override;         // ĸ�� ��� ��� ���
private:
    virtual int Load(const wstring& _strFilePath) override;

public:
    // _PipelineStage : PIPELINE_STAGE
    void UpdateData(int _iRegisterNum, int _PipelineStage);
    void UpdateData_CS(int _iRegisterNum, bool _bShaderRes);
    void Clear();
    void Clear_CS(bool _bShaderRes);
    static void Clear(int _iRegisterNum);

    void DeleteTex() { delete this; }

private:
    virtual void UpdateData() override;

public:
    float Width() { return (float)m_Desc.Width; }
    float Height() { return (float)m_Desc.Height; }

    ComPtr<ID3D11Texture2D>             GetTex2D() { return m_Tex2D; }
    ComPtr<ID3D11ShaderResourceView>    GetSRV() { return m_SRV; }
    ComPtr<ID3D11RenderTargetView>	    GetRTV() { return m_RTV; }
    ComPtr<ID3D11DepthStencilView>	    GetDSV() { return m_DSV; }
    ComPtr<ID3D11UnorderedAccessView>   GetUAV() { return m_UAV; }
};

