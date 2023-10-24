#include "pch.h"
#include "CPrefab.h"

#include "CGameObject.h"
#include "CPathManager.h"
#include "CLevelManager.h"
#include "CLevel.h"
#include "CLayer.h"

CPrefab::CPrefab()
	: CResource(RES_TYPE::PREFAB)
	, m_ProtoObj(nullptr)
{
}

CPrefab::~CPrefab()
{
	if (nullptr != m_ProtoObj)
		delete m_ProtoObj;
}

CGameObject* CPrefab::Instantiate()
{
	// ���� ����� ����� ���� ������Ʈ�� ��������
		return m_ProtoObj->Clone();
}

void CPrefab::AddToLayer(int _iLayerIdx)
{
	CLevel* pCurLevel = CLevelManager::GetInst()->GetCurLevel();
	CGameObject* pProtoObj = Instantiate();
	pCurLevel->AddGameObject(pProtoObj, _iLayerIdx, false);
}

void CPrefab::AddToLayer(wstring _LayerName)
{
	CLevel* pCurLevel = CLevelManager::GetInst()->GetCurLevel();
	CGameObject* pProtoObj = Instantiate();
	pCurLevel->AddGameObject(pProtoObj, _LayerName, false);
}

int CPrefab::Load(const wstring& _strFilePath)
{
	return 0;
}

int CPrefab::Save(const wstring& _strRelativePath)
{
	//wstring strFilePath = CPathManager::GetInst()->GetContentPath();
	//strFilePath += L"prefab\\";
	//strFilePath += _strRelativePath;

	//FILE* pFile = nullptr;
	//_wfopen_s(&pFile, strFilePath.c_str(), L"wb");

	//// Entity -  Name
	//SaveWString(GetName(), pFile);

	//// Resource - Key��
	//SaveWString(GetKey(), pFile);

	//// Shader (Shader �������� + Ű��, ��ΰ�)
	//SaveResourceRef(m_pShader.Get(), pFile);

	//// Constant (��� ���ۿ� ���ε� �� �����) --> ����ü ��°�� ����
	//fwrite(&m_Const, sizeof(tMtrlConst), 1, pFile);

	//// Texture (Tex ������ŭ �ݺ���)
	//for (UINT i = 0; i < (UINT)TEX_PARAM::TEX_END; ++i)
	//{
	//	SaveResourceRef(m_arrTex[i].Get(), pFile);	// ���� Ÿ���� CResource�̹Ƿ� .Get()���� ����
	//}

	//fclose(pFile);

	//return S_OK;
	return S_OK;
}

void CPrefab::RegisterProtoObject(CGameObject* _Proto)
{
	// ���������� ����� ���� ������Ʈ�� ���̾ �ҼӵǸ� �ȵȴ� (���� �� ��ġ �Ұ���)
	assert(-1 == _Proto->GetLayerIdx());

	m_ProtoObj = _Proto;
}
