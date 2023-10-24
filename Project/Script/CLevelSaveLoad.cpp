#include "pch.h"
#include "CLevelSaveLoad.h"

#include <Engine\CPathManager.h>
#include <Engine\CLevelManager.h>
#include <Engine\CLevel.h>
#include <Engine\CLayer.h>
#include <Engine\CGameObject.h>
#include <Engine\component.h>
#include <Engine\CScript.h>

#include "CScriptManager.h"

int CLevelSaveLoad::SaveLevel(const wstring& _LevelPath, CLevel* _Level)
{
    // 레벨을 저장하기 위해서는 무조건 STOP 상태여야한다. -> 따라서 레벨상태 저장 필요없음
    if (_Level->GetState() != LEVEL_STATE::STOP)
        return E_FAIL;

    // 입력으로 들어온 상대경로에 컨텐츠 경로를 붙여 최종경로를 만든다
    wstring strPath = CPathManager::GetInst()->GetContentPath();
    strPath += _LevelPath;

    // 해당 경로로 파일을 생성 후 쓰기모드로 열기
    FILE* pFile = nullptr;
    _wfopen_s(&pFile, strPath.c_str(), L"wb");

    if (nullptr == pFile)
        return E_FAIL;

    // 1. 레벨 이름 저장
    SaveWString(_Level->GetName(), pFile);

    // 2. 레벨의 레이어들을 저장
    for (UINT i = 0; i < MAX_LAYER; ++i)
    {
        CLayer* pLayer = _Level->GetLayer(i);

        // 2-1. 레이어 이름 저장
        SaveWString(pLayer->GetName(), pFile);

        // 2-2. 레이어 내의 게임오브젝트 개수 저장
        const vector<CGameObject*>& vecParent = pLayer->GetParentObject();
        size_t objCount = vecParent.size();
        fwrite(&objCount, sizeof(size_t), 1, pFile);

        // 2-3. 오브젝트 개수만큼 반복문을 돌며 각 게임오브젝트 저장
        for (size_t i = 0; i < objCount; ++i)
        {
            SaveGameObject(vecParent[i], pFile);
        }
    }

    fclose(pFile);

    return S_OK;
}

int CLevelSaveLoad::SaveGameObject(CGameObject* _Object, FILE* _File)
{
    // 1. 이름
    SaveWString(_Object->GetName(), _File);

    // 2. 보유 컴포넌트
    for (UINT i = 0; i <= (UINT)COMPONENT_TYPE::END; ++i)
    {
        // 컴포넌트의 끝을 알기 위해 END값을 저장한다 -> 이 값을 읽으면 끝이라 파악하고 로드 종료
        if (i == (UINT)COMPONENT_TYPE::END)
        {
            // 컴포넌트 타입 저장
            fwrite(&i, sizeof(UINT), 1, _File);
            break;
        }

        CComponent* Com = _Object->GetComponent((COMPONENT_TYPE)i);
        if (nullptr == Com)
            continue;

        // 2-1. 컴포넌트 타입 저장
        fwrite(&i, sizeof(UINT), 1, _File);

        // 2-2. 컴포넌트 정보 저장(각 컴포넌트마다 구현)
        Com->SaveToLevelFile(_File);
    }

    // 3. 스크립트	

    // 3-1. 오브젝트가 가진 스크립트의 총 개수 저장
    const vector<CScript*>& vecScript = _Object->GetScripts();
    size_t ScriptCount = vecScript.size();
    fwrite(&ScriptCount, sizeof(size_t), 1, _File);

    // 3-2. 스크립트 개수만큼 반복문을 돌며 각각 스크립트에 구현된 Save 호출
    for (size_t i = 0; i < vecScript.size(); ++i)
    {
        // 스크립트 이름 저장
        wstring ScriptName = CScriptManager::GetScriptName(vecScript[i]);
        SaveWString(ScriptName, _File);

        // 스크립트 고유의 Save함수 호출
        vecScript[i]->SaveToLevelFile(_File);
    }


    // 4. 자식 오브젝트

    // 4-1. 자식 수 저장
    const vector<CGameObject*>& vecChild = _Object->GetChild();
    size_t ChildCount = vecChild.size();
    fwrite(&ChildCount, sizeof(size_t), 1, _File);

    // 4-2. 자식 수만큼 반복문 돌며 재귀호출
    for (size_t i = 0; i < ChildCount; ++i)
    {
        CGameObject* pChild = vecChild[i];
        // Save the layer index for child objects
        int ChildLayerIdx = pChild->GetLayerIdx();
        fwrite(&ChildLayerIdx, sizeof(int), 1, _File);

        SaveGameObject(vecChild[i], _File);
    }

    return 0;
}

int CLevelSaveLoad::SavePrefab(const wstring& _prefabPath, CGameObject* _protoObj)
{
    // 입력으로 들어온 경로에 콘텐츠 경로를 붙여 불러올 파일의 경로 완성시키기
    wstring strPath = CPathManager::GetInst()->GetContentPath();
    strPath += L"prefab\\";
    strPath += _prefabPath;

    FILE* pFile = nullptr;
    _wfopen_s(&pFile, strPath.c_str(), L"wb");

    // Save Prefab key
    wstring key = L"prefab\\" + _prefabPath;
    SaveWString(key, pFile);

    // Save Prefab Object
    SaveGameObject(_protoObj, pFile);

    DestroyObject(_protoObj);

    fclose(pFile);

    return S_OK;
}

CLevel* CLevelSaveLoad::LoadLevel(const wstring& _LevelPath)
{
    // 입력으로 들어온 경로에 콘텐츠 경로를 붙여 불러올 파일의 경로 완성시키기
    wstring strPath = CPathManager::GetInst()->GetContentPath();
    strPath += _LevelPath;

    // 파일 읽기 모드로 열기
    FILE* pFile = nullptr;
    _wfopen_s(&pFile, strPath.c_str(), L"rb");
    if (nullptr == pFile)
        return nullptr;

    // 0. 레벨 생성
    CLevel* NewLevel = new CLevel;

    // 1. 레벨 이름 불러온 후 세팅
    wstring strLevelName;
    LoadWString(strLevelName, pFile);
    NewLevel->SetName(strLevelName);

    // 2. 레벨 불러오기
    for (UINT i = 0; i < MAX_LAYER; ++i)
    {
        // 2-1. 레이어 이름 불러온 후 세팅
        CLayer* pLayer = NewLevel->GetLayer(i);
        wstring LayerName;
        LoadWString(LayerName, pFile);
        pLayer->SetName(LayerName);

        // 2-2. 레이어 내의 오브젝트 개수 불러오기
        size_t objCount = 0;
        fread(&objCount, sizeof(size_t), 1, pFile);

        // 2-3. 레이어 내 오브젝트 개수만큼 반복문 돌며 오브젝트 불러오기
        for (size_t j = 0; j < objCount; ++j)
        {
            CGameObject* pNewObj = LoadGameObject(pFile);
            NewLevel->AddGameObject(pNewObj, i, false);
        }
    }

    fclose(pFile);

    // 불러온 레벨의 상태를 Stop으로 전환
    NewLevel->ChangeState(LEVEL_STATE::STOP);

    return NewLevel;
}

CGameObject* CLevelSaveLoad::LoadGameObject(FILE* _File)
{
    CGameObject* pObject = new CGameObject;

    // 1. 오브젝트 이름
    wstring Name;
    LoadWString(Name, _File);
    pObject->SetName(Name);

    // 2. 보유 컴포넌트
    while (true)
    {
        UINT ComponentType = 0;
        fread(&ComponentType, sizeof(UINT), 1, _File);

        // 컴포넌트 정보의 끝(END)을 확인하면 while문 탈출
        if ((UINT)COMPONENT_TYPE::END == ComponentType)
            break;

        CComponent* Component = nullptr;

        // 읽어온 컴포넌트 타입(정수)에 따라 컴포넌트를 동적할당한다.
        switch ((COMPONENT_TYPE)ComponentType)
        {
        case COMPONENT_TYPE::TRANSFORM:
            Component = new CTransform;
            break;
        case COMPONENT_TYPE::COLLIDER2D:
            Component = new CCollider2D;
            break;
        case COMPONENT_TYPE::COLLIDER3D:
            //Component = new CCollider3D;
            break;
        case COMPONENT_TYPE::ANIMATOR2D:
            Component = new CAnimator2D;
            break;
        case COMPONENT_TYPE::ANIMATOR3D:
            break;
        case COMPONENT_TYPE::LIGHT2D:
            Component = new CLight2D;
            break;
        case COMPONENT_TYPE::LIGHT3D:
            break;
        case COMPONENT_TYPE::CAMERA:
            Component = new CCamera;
            break;
        case COMPONENT_TYPE::RIGIDBODY2D:
            Component = new CRigidBody2D;
            break;
        case COMPONENT_TYPE::FSM_AI:
            Component = new CFSM;
            break;
        case COMPONENT_TYPE::MESHRENDER:
            Component = new CMeshRender;
            break;
        case COMPONENT_TYPE::PARTICLESYSTEM:
            Component = new CParticleSystem;
            break;
        case COMPONENT_TYPE::TILEMAP:
            Component = new CTileMap;
            break;
        case COMPONENT_TYPE::LANDSCAPE:
            break;
        case COMPONENT_TYPE::DECAL:
            break;
        }

        // 각 컴포넌트마다 불러올 데이터 로드
        Component->LoadFromLevelFile(_File);

        // 오브젝트에 추가
        pObject->AddComponent(Component);
    }

    // 3. 보유 스크립트	

    // 3-1. 스크립트 개수 불러오기
    size_t ScriptCount = 0;
    fread(&ScriptCount, sizeof(size_t), 1, _File);

    // 3-2. 보유한 스크립트 개수만큼 반복문 돌며 스크립트 불러오기
    for (size_t i = 0; i < ScriptCount; ++i)
    {
        // 스크립트 이름 불러온 후, 스크립트매니저에서 해당 이름으로 스크립트 동적할당
        wstring ScriptName;
        LoadWString(ScriptName, _File);
        CScript* pScript = CScriptManager::GetScript(ScriptName);

        // 오브젝트에 스크립트 추가 후 스크립트 고유의 데이터 불러오기
        pObject->AddComponent(pScript);
        pScript->LoadFromLevelFile(_File);
    }

    // 4. 자식 오브젝트

    // 4-1. 자식 개수 불러오기
    size_t ChildCount = 0;
    fread(&ChildCount, sizeof(size_t), 1, _File);

    // 4-2. 보유한 자식 개수만큼 반복문 돌며 LoadGameObj 재귀호출
    for (size_t i = 0; i < ChildCount; ++i)
    {
        // Load the layer index for child objects
        int ChildLayerIdx = 0;
        fread(&ChildLayerIdx, sizeof(int), 1, _File);

        CGameObject* ChildObject = LoadGameObject(_File);

        // 자식과 부모 연결
        pObject->AddChild(ChildObject);
        // 자식 레이어 변경
        ChildObject->ChangeLayer(ChildLayerIdx);
    }

    return pObject;
}

Ptr<CPrefab> CLevelSaveLoad::LoadPrefab(const wstring& _prefabPath)
{
    // 리소스를 추가하기 전 같은 키값으로 들어온 리소스가 있는지 검사한다(중복체크)
    Ptr<CPrefab> pRes = CResourceManager::GetInst()->FindRes<CPrefab>(_prefabPath).Get();

    // 해당 키값으로 이미 저장되어있는 리소스가 있다면, 그걸 반환한다.
    if (nullptr != pRes)
        return (CPrefab*)pRes.Get();

    // 입력으로 들어온 경로에 콘텐츠 경로를 붙여 불러올 파일의 경로 완성시키기
    wstring strPath = CPathManager::GetInst()->GetContentPath();
    strPath += _prefabPath;

    FILE* pFile = nullptr;
    _wfopen_s(&pFile, strPath.c_str(), L"rb");

    Ptr<CPrefab> newPrefab = new CPrefab;

    wstring key;
    LoadWString(key, pFile);
    //newPrefab->SetKey(key);

    CGameObject* protoObj = LoadGameObject(pFile);
    newPrefab->RegisterProtoObject(protoObj);

    CResourceManager::GetInst()->AddRes(key, newPrefab);

    return newPrefab;
}
