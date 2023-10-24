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
    //스마트 포인터는 템플릿 클래스이므로 객체가 소멸될 때 소멸자에서 본인의 해제 함수를 알아서 호출해주므로
    //따로 Release 함수를 적어주지 않아도 된다.

    Safe_Del_Array(m_arrConstBuffer);
}

int CDevice::init(HWND _hWnd, UINT _iWidth, UINT _iHeight)
{
    m_hWnd = _hWnd;
    m_vRenderResolution = Vec2((float)_iWidth, (float)_iHeight);

    GlobalData.Resolution = m_vRenderResolution;

    //디버그 모드에서 실행할 경우 디버그 플래그(에러 잡아줌), 릴리즈 모드일 경우 플래그 없음(0)
    int iFlag = 0;
#ifdef _DEBUG
    iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL eLevel = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;

    // Device 생성
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
        MessageBox(nullptr, L"Device, Context 생성 실패", L"Device 초기화 에러", MB_OK);
        return E_FAIL;
    }

    // SwapChain 생성
    if (FAILED(CreateSwapChain()))
    {
        MessageBox(nullptr, L"SwapChain 생성 실패", L"Device 초기화 에러", MB_OK);
        return E_FAIL;
    }

    // View 생성
    if (FAILED(CreateView()))
    {
        MessageBox(nullptr, L"View 생성 실패", L"Device 초기화 에러", MB_OK);
        return E_FAIL;
    }

    //ViewPort 설정
    m_ViewPort.TopLeftX = 0.f;
    m_ViewPort.TopLeftY = 0.f;

    Vec2 vResolution = CEngine::GetInst()->GetWindowResolution();
    m_ViewPort.Width = vResolution.x;
    m_ViewPort.Height = vResolution.y;

    m_ViewPort.MinDepth = 0.f;
    m_ViewPort.MaxDepth = 1.f;

    m_Context->RSSetViewports(1, &m_ViewPort);

    // RasterizerState 생성
    if (FAILED(CreateRasterizerState()))
    {
        MessageBox(nullptr, L"RasterizerState 생성 실패", L"Device 초기화 에러", MB_OK);
        return E_FAIL;
    }

    // DepthStencilState 생성
    if (FAILED(CreateDepthStencilState()))
    {
        MessageBox(nullptr, L"DepthStencilState 생성 실패", L"Device 초기화 에러", MB_OK);
        return E_FAIL;
    }

    // BlendState 생성
    if (FAILED(CreateBlendState()))
    {
        MessageBox(nullptr, L"BlendState 생성 실패", L"Device 초기화 에러", MB_OK);
        return E_FAIL;
    }

    // 샘플러 생성
	if (FAILED(CreateSampler()))
	{
		MessageBox(nullptr, L"Sampler 생성 실패", L"Device 초기화 에러", MB_OK);
			return E_FAIL;
	}

    // 상수 버퍼 생성
    CreateConstBuffer();

    return S_OK; // E_FAIL;

}

int CDevice::CreateSwapChain()
{
    // 스왑체인 구조체 설정
    DXGI_SWAP_CHAIN_DESC tDesc = {};

    tDesc.OutputWindow = m_hWnd;    // 출력 윈도우
    tDesc.Windowed = true;          // 창모드, 전체화면 모드

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

    // 스왚체인 생성
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
    // =====RTTex(Render Target Texture)와 RTV(Render Target View) 생성=====

    //만들어진 스왑체인에서 Texture2D를 뽑아와서 tex2D에 넣어준다.
    ComPtr<ID3D11Texture2D> tex2D;
    m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)tex2D.GetAddressOf());
    
    // 위에서 뽑아낸 Texture2D로 새로운 Texture를 생성해서 멤버에 대입한다.
    m_RTTex = CResourceManager::GetInst()->CreateTexture(L"RenderTargetTex", tex2D);

    // 이제 CTexture 클래스에서 본인의 플래그에 따라 자동으로 View를 생성 및 관리하므로, Device에서는 더이상 View를 생성하지 않는다.

    // =====DSTex(Depth Stencil Texture)와 DSV(Depth Stencil Texture View) 생성

    // 해상도와 동일한 크기로, DS용 텍스처를 만들어준다.
    m_DSTex = CResourceManager::GetInst()->CreateTexture(L"DepthStencilTex"
        , (UINT)m_vRenderResolution.x, (UINT)m_vRenderResolution.y
        , DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT);
    return S_OK;
}

int CDevice::CreateRasterizerState()
{
    // =========   Culling Mode와 외적/왼손 법칙          ===============================

    // 기본적으로 DirectX는 최적화를 위해 시계방향으로 그려지는 정점의 면만을 채워서 렌더링하고, 반시계방향일 경우 렌더링하지 않는다.
    // 이는 일반적으로 3D환경에서 우리가 바라보는 물체의 외부(껍데기)가 시계 방향 렌더링이고, 내부(속)이 반시계이기 때문에
    // 굳이 외부(껍데기) 면으로 가려질 내부(속) 면을 그리지 않는 것이다.

    // 컴퓨터가 정점의 방향이 시계방향인지 반시계방향인지 알 수 있는 방법은 바로 정점들의 외적을 구하는 것이다.
    // DirectX는 외적의 왼손 법칙을 사용한다. 즉, 정점이 시계방향일 때(네 손가락을 시계방향으로 말아쥘 때) 외적은 위쪽(엄지방향)
    // 정점이 반시계방향일 때에는 손을 거꾸로 뒤집어 외적이 아래쪽 방향을 향한다.
    // 외적은 두 벡터에 모두 수직인 또 하나의 벡터(방향을 가짐)이다.

    // 이런 식으로 외적을 구했을 경우, 시계방향일 때의 외적의 방향을 다이렉트에서는 앞면이라고 보고,
    // 반시계 방향의 외적을 뒷면이라고 판단하여 앞면만을 렌더링하는 모드가 기본적인 래스터라이저의 모드, CULL_BACK 모드이다.
    // 3D로 들어가면 이렇게 구한 정점 메쉬의 외적과 카메라 시야 각도를 서로 내적하여 그 값에 따라 렌더링 여부를 결정한다.

    // =================================================================================

    // 아래 옵션은 RasterizerState를 설정하지 않으면(nullptr) 기본으로 설정되는 기본 옵션(CULL_BACK)이므로, 굳이 작성할 필요가 없다
    // Desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;     // 반시계방향 렌더링 안함
    // Desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;    // 내부 면 채워서 렌더링

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
    Desc.DepthEnable = true;                            // 깊이 판정을 할 것인가
    Desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;       // 기존 깊이보다 가깝거나 같으면 출력함
    Desc.StencilEnable = false;                         // Stencil 옵션 : 사용하지 않으므로 false
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;   // 판정할 깊이를 기록할 것인지
    DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::LESS_EQUAL].GetAddressOf());

    // GREATER
    Desc.DepthEnable = true;                            // 깊이 판정을 할 것인가
    Desc.DepthFunc = D3D11_COMPARISON_GREATER;          // 기존 깊이보다 깊으면 출력함
    Desc.StencilEnable = false;                         // Stencil 옵션 : 사용하지 않으므로 false
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;   // 판정할 깊이를 기록할 것인지
    DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::GREATER].GetAddressOf());

    // GREATER_EQUAL
    Desc.DepthEnable = true;                            // 깊이 판정을 할 것인가
    Desc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;    // 기존 깊이보다 깊거나 같으면 출력함
    Desc.StencilEnable = false;                         // Stencil 옵션 : 사용하지 않으므로 false
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;   // 판정할 깊이를 기록할 것인지
    DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::GREATER_EQUAL].GetAddressOf());

    // NO_WRITE
    Desc.DepthEnable = true;                            // 깊이 판정을 할 것인가
    Desc.DepthFunc = D3D11_COMPARISON_LESS;             // 기존 깊이보다 가깝거나 같으면 출력함
    Desc.StencilEnable = false;                         // Stencil 옵션 : 사용하지 않으므로 false
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;  // 판정할 깊이를 기록하지 않는다
    DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::NO_WRITE].GetAddressOf());

    // NO_TEST_NO_WRITE
    Desc.DepthEnable = false;                           // 깊이 판정을 하지 않는다
    Desc.StencilEnable = false;                         // Stencil 옵션 : 사용하지 않으므로 false
    Desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;  // 판정할 깊이를 기록하지 않는다
    DEVICE->CreateDepthStencilState(&Desc, m_DSState[(UINT)DS_TYPE::NO_TEST_NO_WRITE].GetAddressOf());

    return S_OK;
}

int CDevice::CreateBlendState()
{
    // DEFAULT (NO Blend)
    m_BSState[(UINT)BS_TYPE::DEFAULT] = nullptr;

    D3D11_BLEND_DESC Desc = {};

    // Mask

    // AlphaToCoverage 옵션이란? 반투명 물체의 경우 깊이를 남기지 않는 등의 옵션 + 렌더 순서 정렬로 렌더링 문제를 해결하면 되지만
    // 3D에서 나뭇잎 등의 자잘한 물체가 서로를 뚫고 겹쳐있을 경우 어떤 것이 앞에 그려져있는지 판단하기 어려워진다.
    // 따라서 이 옵션을 켜면 0.4 이하의 알파값은 아예 없는 셈치고 그려버리지 않기 때문에 
    // 투명 혹은 반투명한 재질로 뒤 오브젝트를 가리는 일이 없게된다
    
    // AlphatoCoverage 옵션과 비슷한 개념으로 clip, 혹은 discard 함수가 있는데,
    // PS 단계에서 픽셀들을 전부 날려버리는(소실시키는) 함수이다 (보통 if문으로 알파값이 0이라는 조건하에 사용)
    // 이 경우 소실된 픽셀들은 깊이도 남기지 않고 아예 사라진다
    
    // 그려질 물체가 완전히 투명하거나 완전히 불투명할 때 사용 (반투명/투명한 재질은 AlphaBlend & NOWRITE옵션 사용)

    Desc.AlphaToCoverageEnable = true;              
    Desc.IndependentBlendEnable = false;
    Desc.RenderTarget[0].BlendEnable = false;
    
    Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;  // RGB 색상 중 마스킹 할(거를) 색상 : 안씀.

    DEVICE->CreateBlendState(&Desc, m_BSState[(UINT)BS_TYPE::MASK].GetAddressOf());

    // Alpha Blend
    Desc.AlphaToCoverageEnable = false;
    Desc.IndependentBlendEnable = false;
    Desc.RenderTarget[0].BlendEnable = true;
    
    Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;              // 가산 혼합
    Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;          // 출력할 픽셀의 알파값만큼
    Desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;     // 1에서 출력할 픽셀의 알파값만큼 뺀 값을 뒷배경 

    Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    
    Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;  // RGB 색상 중 마스킹 할(거를) 색상 : 안씀.

    DEVICE->CreateBlendState(&Desc, m_BSState[(UINT)BS_TYPE::ALPHA_BLEND].GetAddressOf());

    // ONE_ONE
    Desc.AlphaToCoverageEnable = false;
    Desc.IndependentBlendEnable = false;
    Desc.RenderTarget[0].BlendEnable = true;

    Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;        // 가산 혼합
    Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;          // 출력할 픽셀 색상에도 1 곱함
    Desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;         // 뒷배경 픽셀 색상에도 1 곱함

    Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

    Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;  // RGB 색상 중 마스킹 할(거를) 색상 : 안씀.

    DEVICE->CreateBlendState(&Desc, m_BSState[(UINT)BS_TYPE::ONE_ONE].GetAddressOf());

    return S_OK;
}

int CDevice::CreateSampler()
{
    D3D11_SAMPLER_DESC tSamDesc = {};

    tSamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;      // Wrap : 바둑판식, MIRROR : 거울처럼 상하좌우반전
    tSamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; 
    tSamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    tSamDesc.Filter = D3D11_FILTER_ANISOTROPIC;         // 이방성 필터링 옵션 (색상을 뭉개서 번진듯한 색상)

    DEVICE->CreateSamplerState(&tSamDesc, m_Sampler[0].GetAddressOf());

    tSamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;      // Wrap : 바둑판식, MIRROR : 거울처럼 상하좌우반전
    tSamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    tSamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    tSamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;    // 필터링 옵션 없음 (정확하게 그 지점의 색상을 집어서 가져옴)

    DEVICE->CreateSamplerState(&tSamDesc, m_Sampler[1].GetAddressOf());

    // 샘플러 바인딩
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
    m_arrConstBuffer[(UINT)CB_TYPE::GLOBAL]->Create(sizeof(tGlobal), 1);    // 광원정보 10개만큼 + int 광원갯수 + 패딩

    return S_OK;
}


void CDevice::ClearTarget(float(&_color)[4])
{
    //RTTex에 주어진 색상으로 깨끗하게 Clear
    m_Context->ClearRenderTargetView(m_RTTex->GetRTV().Get(), _color);

    //DSTex의 깊이 정보를 최대 깊이(1)로, 스텐실 정보를 0으로 초기화.
    m_Context->ClearDepthStencilView(m_DSTex->GetDSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}
