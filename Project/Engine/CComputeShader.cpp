#include "pch.h"
#include "CComputeShader.h"

#include "CPathManager.h"
#include "CDevice.h"
#include "CConstBuffer.h"

CComputeShader::CComputeShader()
	:CShader(RES_TYPE::COMPUTE_SHADER)
	, m_iGroupX(1)
	, m_iGroupY(1)
	, m_iGroupZ(1)
{
}

CComputeShader::~CComputeShader()
{
}

void CComputeShader::CreateComputeShader(const wstring& _strFileName, const string& _strFuncName)
{
	// Shader 파일 경로
	wstring strShaderFile = CPathManager::GetInst()->GetContentPath();
	strShaderFile += _strFileName;

	// Shader Compile	
	if (FAILED(D3DCompileFromFile(strShaderFile.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _strFuncName.c_str(), "cs_5_0", 0, 0, m_CSBlob.GetAddressOf(), m_ErrBlob.GetAddressOf())))
	{
		MessageBoxA(nullptr, (const char*)m_ErrBlob->GetBufferPointer()
			, "Compute Shader Compile Failed!!", MB_OK);
	}

	// 컴파일된 객체로 Shader 를 만든다.
	DEVICE->CreateComputeShader(m_CSBlob->GetBufferPointer(), m_CSBlob->GetBufferSize()
		, nullptr, m_CS.GetAddressOf());
}

void CComputeShader::Execute()
{
	// 자식의 UpdateData 함수 호출
	UpdateData();

	// 컴퓨트 셰이더가 사용할 상수 버퍼 생성 후 상수 버퍼 내 데이터를 컴퓨트셰이더에 바인딩
	static CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::MATERIAL);
	pCB->SetData(&m_Const);
	pCB->UpdateData_CS();

	// 컴퓨트셰이더 설정 후 Dispatch
	CONTEXT->CSSetShader(m_CS.Get(), nullptr, 0);
	CONTEXT->Dispatch(m_iGroupX, m_iGroupY, m_iGroupZ);

	// 자식의 UpdateData 함수 호출
	Clear();
}
