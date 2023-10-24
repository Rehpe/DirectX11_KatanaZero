#include "pch.h"
#include "CMaterial.h"

#include "CDevice.h"
#include "CConstBuffer.h"
#include "CPathManager.h"

CMaterial::CMaterial(bool _bEngine)
	: CResource(RES_TYPE::MATERIAL, _bEngine)
	, m_Const{}
	, m_arrTex{}
{
}

CMaterial::~CMaterial()
{
}

void CMaterial::SetScalarParam(SCALAR_PARAM _Param, const void* _Src)
{
	switch (_Param)
	{
	case INT_0:
	case INT_1:
	case INT_2:
	case INT_3:
		m_Const.arrInt[_Param] = *((int*)_Src);
		break;

	case FLOAT_0:
	case FLOAT_1:
	case FLOAT_2:
	case FLOAT_3:
		m_Const.arrFloat[_Param - FLOAT_0] = *((float*)_Src);
		break;

	case VEC2_0:
	case VEC2_1:
	case VEC2_2:
	case VEC2_3:
		m_Const.arrV2[_Param - VEC2_0] = *((Vec2*)_Src);
		break;

	case VEC4_0:
	case VEC4_1:
	case VEC4_2:
	case VEC4_3:
		m_Const.arrV4[_Param - VEC4_0] = *((Vec4*)_Src);
		break;

	case MAT_0:
	case MAT_1:
	case MAT_2:
	case MAT_3:
		m_Const.arrMat[_Param - MAT_0] = *((Matrix*)_Src);
		break;
	}
}

void CMaterial::GetScalarParam(SCALAR_PARAM _param, void* _pData)
{
	switch (_param)
	{
	case INT_0:
	case INT_1:
	case INT_2:
	case INT_3:
	{
		int idx = (UINT)_param - (UINT)INT_0;
		*((int*)_pData) = m_Const.arrInt[idx];
	}
	break;
	case FLOAT_0:
	case FLOAT_1:
	case FLOAT_2:
	case FLOAT_3:
	{
		int idx = (UINT)_param - (UINT)FLOAT_0;
		*((float*)_pData) = m_Const.arrFloat[idx];
	}
	break;

	case VEC2_0:
	case VEC2_1:
	case VEC2_2:
	case VEC2_3:
	{
		int idx = (UINT)_param - (UINT)VEC2_0;
		*((Vec2*)_pData) = m_Const.arrV2[idx];
	}
	break;

	case VEC4_0:
	case VEC4_1:
	case VEC4_2:
	case VEC4_3:
	{
		int idx = (UINT)_param - (UINT)VEC4_0;
		*((Vec4*)_pData) = m_Const.arrV4[idx];
	}
	break;

	case MAT_0:
	case MAT_1:
	case MAT_2:
	case MAT_3:
	{
		int idx = (UINT)_param - (UINT)MAT_0;
		*((Matrix*)_pData) = m_Const.arrMat[idx];
	}
	break;
	}
}

void CMaterial::SetTexParam(TEX_PARAM _Param, const Ptr<CTexture>& _Tex)
{
	m_arrTex[_Param] = _Tex;
}

void CMaterial::UpdateData()
{
	// 참조하고 있는 Shader가 없으면 return
	if (nullptr == m_pShader)
		return;

	m_pShader->UpdateData();

	// Texture Update
	for (UINT i = 0; i < TEX_END; i++)
	{
		if (nullptr == m_arrTex[i])
		{
			m_Const.arrTex[i] = 0;			// 텍스처 바인딩이 되어있지 않다면 Texture bool값 상수를 0으로 셋팅
			CTexture::Clear(i);
			continue;
		}
			
		else
		{
			m_Const.arrTex[i] = 1;			// 텍스처 바인딩이 되어있다면 Texture bool값 상수를 1로 셋팅
			m_arrTex[i]->UpdateData(i,PIPELINE_STAGE::PS_PIXEL);
		}
	}

	// 재질 상수 버퍼를 얻어와 m_Const에 셋팅된 값 전송
	CConstBuffer* pMtrlBuffer = CDevice::GetInst()->GetConstBuffer(CB_TYPE::MATERIAL);
	pMtrlBuffer->SetData(&m_Const);		// 맵, 언맵
	pMtrlBuffer->UpdateData();			// 상수버퍼 레지스터 바인딩
}

// ===================
// File Save / Load
// ===================

int CMaterial::Save(const wstring& _strRelativePath)
{
	// 만약 엔진리소스이면 세이브의 의미가 없기 때문에 return
	if (IsEngineRes())
		return E_FAIL;

	wstring strFilePath = CPathManager::GetInst()->GetContentPath();
	strFilePath += L"material\\";
	strFilePath += _strRelativePath;

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, strFilePath.c_str(), L"wb");

	// Entity -  Name
	SaveWString(GetName(), pFile);

	// Resource - Key값
	SaveWString(GetKey(), pFile);

	// Shader (Shader 참조여부 + 키값, 경로값)
	SaveResourceRef(m_pShader.Get(), pFile);

	// Constant (상수 버퍼에 바인딩 될 상수값) --> 구조체 통째로 저장
	fwrite(&m_Const, sizeof(tMtrlConst), 1, pFile);

	// Texture (Tex 개수만큼 반복문)
	for (UINT i = 0; i < (UINT)TEX_PARAM::TEX_END; ++i)
	{
		SaveResourceRef(m_arrTex[i].Get(), pFile);	// 인자 타입이 CResource이므로 .Get()으로 전달
	}

	fclose(pFile);

	return S_OK;
}

int CMaterial::Load(const wstring& _strFilePath)
{
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");

	// Entity - Name 불러오기
	wstring strName;
	LoadWString(strName, pFile);
	SetName(strName);

	// Resource - Key값 불러오기
	wstring strKey;
	LoadWString(strKey, pFile);
	
	// Shader
	LoadResourceRef(m_pShader, pFile);

	// Constant
	fread(&m_Const, sizeof(tMtrlConst), 1, pFile);

	// Texture
	for (UINT i = 0; i < (UINT)TEX_PARAM::TEX_END; ++i)
	{
		LoadResourceRef(m_arrTex[i], pFile);
	}

	fclose(pFile);

	return S_OK;
}


