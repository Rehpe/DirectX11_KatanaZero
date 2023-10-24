#pragma once
#include "CResource.h"

#include <DirectXTex\DirectXTex.h>

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex//DirectXTex_debug")
#else
#pragma comment(lib, "DirectXTex//DirectXTex")
#endif

// 텍스처는 Texture2D  하나와, 이 텍스처를 전달할 수 있는 다양한 플래그의 View로 이루어져있다.

class CTexture :
    public CResource
{
public:
    CTexture(bool _bEngine = false);
    ~CTexture();

private:
    ComPtr<ID3D11Texture2D>                 m_Tex2D;
    ComPtr<ID3D11ShaderResourceView>        m_SRV;      // 레지스터에 바인딩할 때 사용되는 뷰
    ComPtr<ID3D11RenderTargetView>	        m_RTV;      // 렌더 타겟용 텍스처를 생성할 때 사용되는 뷰
    ComPtr<ID3D11DepthStencilView>	        m_DSV;      // 뎁스스텐실용 텍스처를 생성할 때 사용되는 뷰
    ComPtr<ID3D11UnorderedAccessView>       m_UAV;      // 읽기, 쓰기가 동시에 가능한 텍스처를 생성할 때 사용되는 뷰
    
    D3D11_TEXTURE2D_DESC                    m_Desc;

    ScratchImage                            m_Image;    // 저장장치의 텍스처를 시스템 메모리로 가져오는 것을 담당

    UINT                                    m_iRecentNum;     // 가장 최근 바인딩한 CS 레지스터 번호
public:
    int Create(UINT _Width, UINT _Height, DXGI_FORMAT _pixelformat
        , UINT _BindFlag, D3D11_USAGE _Usage);                      // 새로운 텍스처를 생성할 경우

    int Create(ComPtr<ID3D11Texture2D> _tex2D);                     // Texture2D가 통으로 들어오는 경우(Ex. Device의 RT)

public:
    virtual int Save(const wstring& _strRelativePath) override;         // 캡쳐 기능 등에서 사용
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

