#include "pch.h"
#include "CDevice.h"

#include "CEngine.h"
#include "CConstBuffer.h"
#include "CResourceManager.h"

CDevice::CDevice()
    : m_hWnd(nullptr)
    , m_ViewPort{}
    , m_arrConstBuffer{}
{
}

CDevice::~CDevice()
{
    //����Ʈ �����ʹ� ���ø� Ŭ�����̹Ƿ� ��ü�� �Ҹ�� �� �Ҹ��ڿ��� ������ ���� �Լ��� �˾Ƽ� ȣ�����ֹǷ�
    //���� Release �Լ��� �������� �ʾƵ� �ȴ�.

    Safe_Del_Array(m_arrConstBuffer);
}

int CDevice::init(HWND _hWnd, UINT _iWidth, UINT _iHeight)
{
    m_hWnd = _hWnd;
    m_vRenderResolution = Vec2((float)_iWidth, (float)_iHeight);

    GlobalData.Resolution = m_vRenderResolution;

    //����� ��忡�� ������ ��� ����� �÷���(���� �����), ������ ����� ��� �÷��� ����(0)
    int iFlag = 0;
#ifdef _DEBUG
    iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL eLevel = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;

    // Device ����
    if (FAILED(D3D11CreateDevice(nullptr
        , D3D_DRIVER_TYPE_HARDWARE
        , nullptr
        , iFlag
        , nullptr, 0
        , D3D11_SDK_VERSION
        , m_Device.GetAddressOf()
        , &eLevel
        , m_Context.GetAddressOf())))
    {
        MessageBox(nullptr, L"Device, Context ���� ����", L"Device �ʱ�ȭ ����", MB_OK);
        return E_FAIL;
    }

    // SwapChain ����
    if (FAILED(CreateSwapChain()))
    {
        MessageBox(nullptr, L"SwapChain ���� ����", L"Device �ʱ�ȭ ����", MB_OK);
        return E_FAIL;
    }

    // View ����
    if (FAILED(CreateView()))
    {
        MessageBox(nullptr, L"View ���� ����", L"Device �ʱ�ȭ ����", MB_OK);
        return E_FAIL;
    }

    //ViewPort ����
    m_ViewPort.TopLeftX = 0.f;
    m_ViewPort.TopLeftY = 0.f;

    Vec2 vResolution = CEngine::GetInst()->GetWindowResolution();
    m_ViewPort.Width = vResolution.x;
    m_ViewPort.Height = vResolution.y;

    m_ViewPort.MinDepth = 0.f;
    m_ViewPort.MaxDepth = 1.f;

    m_Context->RSSetViewports(1, &m_ViewPort);

    // RasterizerState ����
    if (FAILED(CreateRasterizerState()))
    {
        MessageBox(nullptr, L"RasterizerState ���� ����", L"Device �ʱ�ȭ ����", MB_OK);
        return E_FAIL;
    }

    // DepthStencilState ����
    if (FAILED(CreateDepthStencilState()))
    {
        MessageBox(nullptr, L"DepthStencilState ���� ����", L"Device �ʱ�ȭ ����", MB_OK);
        return E_FAIL;
    }

    // BlendState ����
    if (FAILED(CreateBlendState()))
    {
        MessageBox(nullptr, L"BlendState ���� ����", L"Device �ʱ�ȭ ����", MB_OK);
        return E_FAIL;
    }

    // ���÷� ����
	if (FAILED(CreateSampler()))
	{
		MessageBox(nullptr, L"Sampler ���� ����", L"Device �ʱ�ȭ ����", MB_OK);
			return E_FAIL;
	}

    // ��� ���� ����
    CreateConstBuffer();

    return S_OK; // E_FAIL;

}

int CDevice::CreateSwapChain()
{
    // ����ü�� ����ü ����
    DXGI_SWAP_CHAIN_DESC tDesc = {};

    tDesc.OutputWindow = m_hWnd;    // ��� ������
    tDesc.Windowed = true;          // â���, ��üȭ�� ���

    tDesc.BufferCount = 1;
    tDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    tDesc.BufferDesc.Width = (UINT)m_vRenderResolution.x;
    tDesc.BufferDesc.Height = (UINT)m_vRenderResolution.y;
    tDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    tDesc.BufferDesc.RefreshRate.Denominator = 1;
    tDesc.BufferDesc.RefreshRate.Numerator = 60;
    tDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    tDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

    tDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;

    tDesc.SampleDesc.Count = 1;
    tDesc.SampleDesc.Quality = 0;
    tDesc.Flags = 0;

    // ����ü�� ����
    ComPtr<IDXGIDevice>  pDXGIDevice;
    ComPtr<IDXGIAdapter> pAdapter;
    ComPtr<IDXGIFactory> pFactory;

    HRESULT hr = S_OK;

    hr = m_Device->QueryInterface(__uuidof(IDXGIDevice), (void**)pDXGIDevice.GetAddressOf());
    hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)pAdapter.GetAddressOf());
    hr = pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)pFactory.GetAddressOf());

    hr = pFactory->CreateSwapChain(m_Device.Get(), &tDesc, m_SwapChain.GetAddressOf());

    return hr;
}

int CDevice::CreateView()
{
    // =====RTTex(Render Target Texture)�� RTV(Render Target View) ����=====

    //������� ����ü�ο��� Texture2D�� �̾ƿͼ� tex2D�� �־��ش�.
    ComPtr<ID3D11Texture2D> tex2D;
    m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)tex2D.GetAddressOf());
    
    // ������ �̾Ƴ� Texture2D�� ���ο� Texture�� �����ؼ� ����� �����Ѵ�.
    m_RTTex = CResourceManager::GetInst()->CreateTexture(L"RenderTargetTex", tex2D);

    // ���� CTexture Ŭ�������� ������ �÷��׿� ���� �ڵ����� View�� ���� �� �����ϹǷ�, Device������ ���̻� View�� �������� �ʴ´�.

    // =====DSTex(Depth Stencil Texture)�� DSV(Depth Stencil Texture View) ����

    // �ػ󵵿� ������ ũ���, DS�� �ؽ�ó�� ������ش�.
    m_DSTex = CResourceManager::GetInst()->CreateTexture(L"DepthStencilTex"
        , (UINT)m_vRenderResolution.x, (UINT)m_vRenderResolution.y
        , DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT);
    return S_OK;
}

int CDevice::CreateRasterizerState()
{
    // =========   Culling Mode�� ����/�޼� ��Ģ          ===============================

    // �⺻������ DirectX�� ����ȭ�� ���� �ð�������� �׷����� ������ �鸸�� ä���� �������ϰ�, �ݽð������ ��� ���������� �ʴ´�.
    // �̴� �Ϲ������� 3Dȯ�濡�� �츮�� �ٶ󺸴� ��ü�� �ܺ�(������)�� �ð� ���� �������̰�, ����(��)�� �ݽð��̱� ������
    // ���� �ܺ�(������) ������ ������ ����(��) ���� �׸��� �ʴ� ���̴�.

    // ��ǻ�Ͱ� ������ ������ �ð�������� �ݽð�������� �� �� �ִ� ����� �ٷ� �������� ������ ���ϴ� ���̴�.
    // DirectX�� ������ �޼� ��Ģ�� ����Ѵ�. ��, ������ �ð������ ��(�� �հ����� �ð�������� ������ ��) ������ ����(��������)
    // ������ �ݽð������ ������ ���� �Ųٷ� ������ ������ �Ʒ��� ������ ���Ѵ�.
    // ������ �� ���Ϳ� ��� ������ �� �ϳ��� ����(������ ����)�̴�.

    // �̷� ������ ������ ������ ���, �ð������ ���� ������ ������ ���̷�Ʈ������ �ո��̶�� ����,
    // �ݽð� ������ ������ �޸��̶�� �Ǵ��Ͽ� �ո鸸�� �������ϴ� ��尡 �⺻���� �����Ͷ������� ���, CULL_BACK ����̴�.
    // 3D�� ���� �̷��� ���� ���� �޽��� ������ ī�޶� �þ� ������ ���� �����Ͽ� �� ���� ���� ������ ���θ� �����Ѵ�.

    // =================================================================================

    // �Ʒ� �ɼ��� RasterizerState�� �������� ������(nullptr) �⺻���� �����Ǵ� �⺻ �ɼ�(CULL_BACK)�̹Ƿ�, ���� �ۼ��� �ʿ䰡 ����
    // Desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;     // �ݽð���� ������ ����
    // Desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;    // ���� �� ä���� ������

    m_RSState[(UINT)RS_TYPE::CULL_BACK] = nullptr;

    D3D11_RASTERIZER_DESC Desc = {};

    // CULL_FRONT
    Desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;     
    Desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    DEVICE->CreateRasterizerState(&Desc, m_RSState[(UINT)RS_TYPE::CULL_FRONT].GetAddressOf());

    // CULL_NONE
    Desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
    Desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    DEVICE->CreateRasterizerState(&Desc, m_RSState[(UINT)RS_TYPE::CULL_NONE].GetAddressOf());

    // WIREFRAME
    Desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
    Desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
    DEVICE->CreateRasterizerState(&Desc, m_RSState[(UINT)RS_TYPE::WIRE_FRAME].GetAddressOf());

    return S_OK;
}

int CDevice::CreateDepthStencilState()
{
    // LESS (Default)
    m_DSState[(UINT)DS_TYPE::LESS] = nullptr;

    D3D11_DEPTH_STENCIL_DESC Desc = {};

    // LESS_EQUAL
    Desc.DepthEnable = true;                            // ���� ������ �� ���ΰ�
    Desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;       // ���� ���̺��� �����ų� ������ �����
    Desc.StencilEnable = false;                         // Stencil �ɼ� : ������� �����Ƿ� false
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;   // ������ ���̸� ����� ������
    DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::LESS_EQUAL].GetAddressOf());

    // GREATER
    Desc.DepthEnable = true;                            // ���� ������ �� ���ΰ�
    Desc.DepthFunc = D3D11_COMPARISON_GREATER;          // ���� ���̺��� ������ �����
    Desc.StencilEnable = false;                         // Stencil �ɼ� : ������� �����Ƿ� false
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;   // ������ ���̸� ����� ������
    DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::GREATER].GetAddressOf());

    // GREATER_EQUAL
    Desc.DepthEnable = true;                            // ���� ������ �� ���ΰ�
    Desc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;    // ���� ���̺��� ��ų� ������ �����
    Desc.StencilEnable = false;                         // Stencil �ɼ� : ������� �����Ƿ� false
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;   // ������ ���̸� ����� ������
    DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::GREATER_EQUAL].GetAddressOf());

    // NO_WRITE
    Desc.DepthEnable = true;                            // ���� ������ �� ���ΰ�
    Desc.DepthFunc = D3D11_COMPARISON_LESS;             // ���� ���̺��� �����ų� ������ �����
    Desc.StencilEnable = false;                         // Stencil �ɼ� : ������� �����Ƿ� false
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;  // ������ ���̸� ������� �ʴ´�
    DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::NO_WRITE].GetAddressOf());

    // NO_TEST_NO_WRITE
    Desc.DepthEnable = false;                           // ���� ������ ���� �ʴ´�
    Desc.StencilEnable = false;                         // Stencil �ɼ� : ������� �����Ƿ� false
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;  // ������ ���̸� ������� �ʴ´�
    DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::NO_TEST_NO_WRITE].GetAddressOf());

    return S_OK;
}

int CDevice::CreateBlendState()
{
    // DEFAULT (NO Blend)
    m_BSState[(UINT)BS_TYPE::DEFAULT] = nullptr;

    D3D11_BLEND_DESC Desc = {};

    // Mask

    // AlphaToCoverage �ɼ��̶�? ������ ��ü�� ��� ���̸� ������ �ʴ� ���� �ɼ� + ���� ���� ���ķ� ������ ������ �ذ��ϸ� ������
    // 3D���� ������ ���� ������ ��ü�� ���θ� �հ� �������� ��� � ���� �տ� �׷����ִ��� �Ǵ��ϱ� ���������.
    // ���� �� �ɼ��� �Ѹ� 0.4 ������ ���İ��� �ƿ� ���� ��ġ�� �׷������� �ʱ� ������ 
    // ���� Ȥ�� �������� ������ �� ������Ʈ�� ������ ���� ���Եȴ�
    
    // AlphatoCoverage �ɼǰ� ����� �������� clip, Ȥ�� discard �Լ��� �ִµ�,
    // PS �ܰ迡�� �ȼ����� ���� ����������(�ҽǽ�Ű��) �Լ��̴� (���� if������ ���İ��� 0�̶�� �����Ͽ� ���)
    // �� ��� �ҽǵ� �ȼ����� ���̵� ������ �ʰ� �ƿ� �������
    
    // �׷��� ��ü�� ������ �����ϰų� ������ �������� �� ��� (������/������ ������ AlphaBlend & NOWRITE�ɼ� ���)

    Desc.AlphaToCoverageEnable = true;              
    Desc.IndependentBlendEnable = false;
    Desc.RenderTarget[0].BlendEnable = false;
    
    Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;  // RGB ���� �� ����ŷ ��(�Ÿ�) ���� : �Ⱦ�.

    DEVICE->CreateBlendState(&Desc, m_BSState[(UINT)BS_TYPE::MASK].GetAddressOf());

    // Alpha Blend
    Desc.AlphaToCoverageEnable = false;
    Desc.IndependentBlendEnable = false;
    Desc.RenderTarget[0].BlendEnable = true;
    
    Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;              // ���� ȥ��
    Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;          // ����� �ȼ��� ���İ���ŭ
    Desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;     // 1���� ����� �ȼ��� ���İ���ŭ �� ���� �޹�� 

    Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    
    Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;  // RGB ���� �� ����ŷ ��(�Ÿ�) ���� : �Ⱦ�.

    DEVICE->CreateBlendState(&Desc, m_BSState[(UINT)BS_TYPE::ALPHA_BLEND].GetAddressOf());

    // ONE_ONE
    Desc.AlphaToCoverageEnable = false;
    Desc.IndependentBlendEnable = false;
    Desc.RenderTarget[0].BlendEnable = true;

    Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;        // ���� ȥ��
    Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;          // ����� �ȼ� ���󿡵� 1 ����
    Desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;         // �޹�� �ȼ� ���󿡵� 1 ����

    Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

    Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;  // RGB ���� �� ����ŷ ��(�Ÿ�) ���� : �Ⱦ�.

    DEVICE->CreateBlendState(&Desc, m_BSState[(UINT)BS_TYPE::ONE_ONE].GetAddressOf());

    return S_OK;
}

int CDevice::CreateSampler()
{
    D3D11_SAMPLER_DESC tSamDesc = {};

    tSamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;      // Wrap : �ٵ��ǽ�, MIRROR : �ſ�ó�� �����¿����
    tSamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; 
    tSamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    tSamDesc.Filter = D3D11_FILTER_ANISOTROPIC;         // �̹漺 ���͸� �ɼ� (������ ������ �������� ����)

    DEVICE->CreateSamplerState(&tSamDesc, m_Sampler[0].GetAddressOf());

    tSamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;      // Wrap : �ٵ��ǽ�, MIRROR : �ſ�ó�� �����¿����
    tSamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    tSamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    tSamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;    // ���͸� �ɼ� ���� (��Ȯ�ϰ� �� ������ ������ ��� ������)

    DEVICE->CreateSamplerState(&tSamDesc, m_Sampler[1].GetAddressOf());

    // ���÷� ���ε�
    CONTEXT->VSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
    CONTEXT->HSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
    CONTEXT->DSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
    CONTEXT->GSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());
    CONTEXT->PSSetSamplers(0, 1, m_Sampler[0].GetAddressOf());

    CONTEXT->VSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
    CONTEXT->HSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
    CONTEXT->DSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
    CONTEXT->GSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
    CONTEXT->PSSetSamplers(1, 1, m_Sampler[1].GetAddressOf());
 
    return S_OK;
}

int CDevice::CreateConstBuffer()
{
    m_arrConstBuffer[(UINT)CB_TYPE::TRANSFORM] = new CConstBuffer((UINT)CB_TYPE::TRANSFORM);
    m_arrConstBuffer[(UINT)CB_TYPE::TRANSFORM]->Create(sizeof(tTransform), 1);

    m_arrConstBuffer[(UINT)CB_TYPE::MATERIAL] = new CConstBuffer((UINT)CB_TYPE::MATERIAL);
    m_arrConstBuffer[(UINT)CB_TYPE::MATERIAL]->Create(sizeof(tMtrlConst), 1);

    m_arrConstBuffer[(UINT)CB_TYPE::GLOBAL] = new CConstBuffer((UINT)CB_TYPE::GLOBAL);
    m_arrConstBuffer[(UINT)CB_TYPE::GLOBAL]->Create(sizeof(tGlobal), 1);    // �������� 10����ŭ + int �������� + �е�

    return S_OK;
}


void CDevice::ClearTarget(float(&_color)[4])
{
    //RTTex�� �־��� �������� �����ϰ� Clear
    m_Context->ClearRenderTargetView(m_RTTex->GetRTV().Get(), _color);

    //DSTex�� ���� ������ �ִ� ����(1)��, ���ٽ� ������ 0���� �ʱ�ȭ.
    m_Context->ClearDepthStencilView(m_DSTex->GetDSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}
