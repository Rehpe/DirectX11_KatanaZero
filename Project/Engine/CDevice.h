#pragma once
#include "ptr.h"
#include "CTexture.h"

class CConstBuffer;


class CDevice
	:public CSingleton<CDevice>
{
public:
	CDevice();
	~CDevice();

private:
	HWND							m_hWnd;

	ComPtr<ID3D11Device>			m_Device;		// GPU 메모리 할당 
	ComPtr<ID3D11DeviceContext>		m_Context;		// GPU 제어, 렌더링, 동작 수행

	ComPtr<IDXGISwapChain>			m_SwapChain;	//스왑체인

	Ptr<CTexture>					m_RTTex;

	Ptr<CTexture>					m_DSTex;
	
	// Sampler
	ComPtr<ID3D11SamplerState>		m_Sampler[2];

	// Rasterizer State
	ComPtr<ID3D11RasterizerState>	m_RSState[(UINT)RS_TYPE::END];

	// DepthStencil State
	ComPtr<ID3D11DepthStencilState> m_DSState[(UINT)DS_TYPE::END];

	// Blend State
	ComPtr<ID3D11BlendState>		m_BSState[(UINT)BS_TYPE::END];


	D3D11_VIEWPORT					m_ViewPort;

	//렌더 타겟 해상도
	Vec2							m_vRenderResolution;
	CConstBuffer*					m_arrConstBuffer[(UINT)CB_TYPE::END];

public:
	int		init(HWND _hWnd, UINT _iWidth, UINT _iHeight);

public:
	void ClearTarget(float(&_color)[4]);
	void Present() { m_SwapChain->Present(0, 0); }
	void OMSet() { m_Context->OMSetRenderTargets(1, m_RTTex->GetRTV().GetAddressOf(), m_DSTex->GetDSV().Get()); }
	Vec2 GetRenderResolution() { return m_vRenderResolution; }

	Ptr<CTexture> GetRTTex() { return m_RTTex; }

public:
	ID3D11Device*		 GetDevice()		{ return m_Device.Get(); };
	ID3D11DeviceContext* GetDeviceContext() { return m_Context.Get(); };
 	CConstBuffer*		 GetConstBuffer(CB_TYPE _Type) { return m_arrConstBuffer[(UINT)_Type]; }
	ComPtr<ID3D11RasterizerState> GetRSState(RS_TYPE _type) { return m_RSState[(UINT)_type]; }
	ComPtr<ID3D11DepthStencilState> GetDSState(DS_TYPE _type) { return m_DSState[(UINT)_type]; }
	ComPtr<ID3D11BlendState> GetBSState(BS_TYPE _type) { return m_BSState[(UINT)_type]; }

private:
	int	CreateSwapChain();
	int CreateView();
	int CreateRasterizerState();
	int CreateDepthStencilState();
	int CreateBlendState();
	int CreateSampler();
	int CreateConstBuffer();

};

