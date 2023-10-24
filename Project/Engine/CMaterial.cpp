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
	// �����ϰ� �ִ� Shader�� ������ return
	if (nullptr == m_pShader)
		return;

	m_pShader->UpdateData();

	// Texture Update
	for (UINT i = 0; i < TEX_END; i++)
	{
		if (nullptr == m_arrTex[i])
		{
			m_Const.arrTex[i] = 0;			// �ؽ�ó ���ε��� �Ǿ����� �ʴٸ� Texture bool�� ����� 0���� ����
			CTexture::Clear(i);
			continue;
		}
			
		else
		{
			m_Const.arrTex[i] = 1;			// �ؽ�ó ���ε��� �Ǿ��ִٸ� Texture bool�� ����� 1�� ����
			m_arrTex[i]->UpdateData(i,PIPELINE_STAGE::PS_PIXEL);
		}
	}

	// ���� ��� ���۸� ���� m_Const�� ���õ� �� ����
	CConstBuffer* pMtrlBuffer = CDevice::GetInst()->GetConstBuffer(CB_TYPE::MATERIAL);
	pMtrlBuffer->SetData(&m_Const);		// ��, ���
	pMtrlBuffer->UpdateData();			// ������� �������� ���ε�
}

// ===================
// File Save / Load
// ===================

int CMaterial::Save(const wstring& _strRelativePath)
{
	// ���� �������ҽ��̸� ���̺��� �ǹ̰� ���� ������ return
	if (IsEngineRes())
		return E_FAIL;

	wstring strFilePath = CPathManager::GetInst()->GetContentPath();
	strFilePath += L"material\\";
	strFilePath += _strRelativePath;

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, strFilePath.c_str(), L"wb");

	// Entity -  Name
	SaveWString(GetName(), pFile);

	// Resource - Key��
	SaveWString(GetKey(), pFile);

	// Shader (Shader �������� + Ű��, ��ΰ�)
	SaveResourceRef(m_pShader.Get(), pFile);

	// Constant (��� ���ۿ� ���ε� �� �����) --> ����ü ��°�� ����
	fwrite(&m_Const, sizeof(tMtrlConst), 1, pFile);

	// Texture (Tex ������ŭ �ݺ���)
	for (UINT i = 0; i < (UINT)TEX_PARAM::TEX_END; ++i)
	{
		SaveResourceRef(m_arrTex[i].Get(), pFile);	// ���� Ÿ���� CResource�̹Ƿ� .Get()���� ����
	}

	fclose(pFile);

	return S_OK;
}

int CMaterial::Load(const wstring& _strFilePath)
{
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");

	// Entity - Name �ҷ�����
	wstring strName;
	LoadWString(strName, pFile);
	SetName(strName);

	// Resource - Key�� �ҷ�����
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


