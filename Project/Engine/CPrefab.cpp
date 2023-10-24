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
	// 본인 멤버로 저장된 원본 오브젝트를 복사해줌
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

	//// Resource - Key값
	//SaveWString(GetKey(), pFile);

	//// Shader (Shader 참조여부 + 키값, 경로값)
	//SaveResourceRef(m_pShader.Get(), pFile);

	//// Constant (상수 버퍼에 바인딩 될 상수값) --> 구조체 통째로 저장
	//fwrite(&m_Const, sizeof(tMtrlConst), 1, pFile);

	//// Texture (Tex 개수만큼 반복문)
	//for (UINT i = 0; i < (UINT)TEX_PARAM::TEX_END; ++i)
	//{
	//	SaveResourceRef(m_arrTex[i].Get(), pFile);	// 인자 타입이 CResource이므로 .Get()으로 전달
	//}

	//fclose(pFile);

	//return S_OK;
	return S_OK;
}

void CPrefab::RegisterProtoObject(CGameObject* _Proto)
{
	// 프리팹으로 저장될 원본 오브젝트는 레이어에 소속되면 안된다 (게임 내 배치 불가능)
	assert(-1 == _Proto->GetLayerIdx());

	m_ProtoObj = _Proto;
}
