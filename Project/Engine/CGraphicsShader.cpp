#include "pch.h"
#include "CGraphicsShader.h"

#include "CPathManager.h"
#include "CDevice.h"

CGraphicsShader::CGraphicsShader()
	: CShader(RES_TYPE::GRAPHICS_SHADER)
	, m_eTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	, m_RSType(RS_TYPE::CULL_BACK)
	, m_DSType(DS_TYPE::LESS)
	, m_BSType(BS_TYPE::DEFAULT)
	, m_Domain(SHADER_DOMAIN::DOMAIN_UNDEFINED)		// 기본값은 Undefined, 반드시 도메인 설정이 필요하기에 
													// 렌더링할 오브젝트의 도메인설정이 undefined라면 추후 assert 처리
{
}

CGraphicsShader::~CGraphicsShader()
{
}

void CGraphicsShader::CreateVertexShader(const wstring& _strFileName, const string& _strFuncName)
{
	// Shader 파일 경로
	wstring strShaderFile = CPathManager::GetInst()->GetContentPath();
	strShaderFile += _strFileName;

	// Vertex Shader Compile
	if (FAILED(D3DCompileFromFile(strShaderFile.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _strFuncName.c_str(), "vs_5_0", 0, 0, m_VSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf())))
	{
		MessageBoxA(nullptr, (const char*)m_ErrBlob->GetBufferPointer()
			, "Vertex Shader Compile Fail", MB_OK);
	}

	// 컴파일된 객체로 VertexShader을 만든다.
	DEVICE->CreateVertexShader(m_VSBlob->GetBufferPointer(), m_VSBlob->GetBufferSize(), nullptr, m_VS.GetAddressOf());

	// InputLayout 설정
	D3D11_INPUT_ELEMENT_DESC LayoutDesc[3] = {};

	LayoutDesc[0].SemanticName = "POSITION";
	LayoutDesc[0].SemanticIndex = 0;
	LayoutDesc[0].AlignedByteOffset = 0;
	LayoutDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	LayoutDesc[0].InputSlot = 0;
	LayoutDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	LayoutDesc[0].InstanceDataStepRate = 0;

	LayoutDesc[1].SemanticName = "COLOR";
	LayoutDesc[1].SemanticIndex = 0;
	LayoutDesc[1].AlignedByteOffset = 12;
	LayoutDesc[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	LayoutDesc[1].InputSlot = 0;
	LayoutDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	LayoutDesc[1].InstanceDataStepRate = 0;

	LayoutDesc[2].SemanticName = "TEXCOORD";
	LayoutDesc[2].SemanticIndex = 0;
	LayoutDesc[2].AlignedByteOffset = 28;
	LayoutDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;			// 8바이트
	LayoutDesc[2].InputSlot = 0;
	LayoutDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	LayoutDesc[2].InstanceDataStepRate = 0;

	if (FAILED(DEVICE->CreateInputLayout(LayoutDesc, 3
		, m_VSBlob->GetBufferPointer(), m_VSBlob->GetBufferSize()
		, m_Layout.GetAddressOf())))
	{
		assert(nullptr);
	}
}

void CGraphicsShader::CreateGeometryShader(const wstring& _strFileName, const string& _strFuncName)
{
	// Shader 파일 경로
	wstring strShaderFile = CPathManager::GetInst()->GetContentPath();
	strShaderFile += _strFileName;

	// Shader Compile	
	if (FAILED(D3DCompileFromFile(strShaderFile.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _strFuncName.c_str(), "gs_5_0", 0, 0, m_GSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf())))
	{
		MessageBoxA(nullptr, (const char*)m_ErrBlob->GetBufferPointer()
			, "Shader Compile Failed!!", MB_OK);
	}

	// 컴파일된 객체로 Shader 를 만든다.
	DEVICE->CreateGeometryShader(m_GSBlob->GetBufferPointer(), m_GSBlob->GetBufferSize()
		, nullptr, m_GS.GetAddressOf());
}

void CGraphicsShader::CreatePixelShader(const wstring& _strFileName, const string& _strFuncName)
{
	// Shader 파일 경로
	wstring strShaderFile = CPathManager::GetInst()->GetContentPath();
	strShaderFile += _strFileName;

	DWORD dwDebugFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	dwDebugFlags |= D3DCOMPILE_DEBUG;
	dwDebugFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;

	// Pixel Shader Compile
	if (FAILED(D3DCompileFromFile(strShaderFile.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _strFuncName.c_str(), "ps_5_0", dwDebugFlags, 0, m_PSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf())))
	{
		MessageBoxA(nullptr, (const char*)m_ErrBlob->GetBufferPointer()
			, "Pixel Shader Compile Fail", MB_OK);
	}

	// 컴파일된 객체로 PixelShader을 만든다.
	DEVICE->CreatePixelShader(m_PSBlob->GetBufferPointer(), m_PSBlob->GetBufferSize(), nullptr, m_PS.GetAddressOf());
}

void CGraphicsShader::UpdateData()
{
	CONTEXT->IASetInputLayout(m_Layout.Get());
	CONTEXT->IASetPrimitiveTopology(m_eTopology);

	// SetShader 함수를 호출하지 않으면 해당 단계의 셰이더 단계 자체가 진행되지 않는다.
	CONTEXT->VSSetShader(m_VS.Get(), nullptr, 0);
	CONTEXT->HSSetShader(m_HS.Get(), nullptr, 0);
	CONTEXT->DSSetShader(m_DS.Get(), nullptr, 0);
	CONTEXT->GSSetShader(m_GS.Get(), nullptr, 0);
 	CONTEXT->PSSetShader(m_PS.Get(), nullptr, 0);

	// SetState 함수의 경우 디폴트값이 있기 때문에 SetShader과는 다르게 적지 않아도 디폴트값이 동작한다.
	CONTEXT->RSSetState(CDevice::GetInst()->GetRSState(m_RSType).Get());
	CONTEXT->OMSetDepthStencilState(CDevice::GetInst()->GetDSState(m_DSType).Get(), 0);	// 두 번째 인자는 Stencil 여부(사용 안하니 0)
	
	// 두 번째 인자는 Blend Factor(우리는 렌더링할 픽셀 색, 배경 색에 alpha, inverse alpha를 곱하기로 했으므로 사용하지 않는다)
	// 세 번째 인자는 모든 RGB값을 출력하겠다는 의미로, 모든 자리에 1을 넣은 비트연산자 0xffffffff
	CONTEXT->OMSetBlendState(CDevice::GetInst()->GetBSState(m_BSType).Get(), Vec4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
}
