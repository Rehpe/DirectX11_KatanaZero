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
    // ������ �����ϱ� ���ؼ��� ������ STOP ���¿����Ѵ�. -> ���� �������� ���� �ʿ����
    if (_Level->GetState() != LEVEL_STATE::STOP)
        return E_FAIL;

    // �Է����� ���� ����ο� ������ ��θ� �ٿ� ������θ� �����
    wstring strPath = CPathManager::GetInst()->GetContentPath();
    strPath += _LevelPath;

    // �ش� ��η� ������ ���� �� ������� ����
    FILE* pFile = nullptr;
    _wfopen_s(&pFile, strPath.c_str(), L"wb");

    if (nullptr == pFile)
        return E_FAIL;

    // 1. ���� �̸� ����
    SaveWString(_Level->GetName(), pFile);

    // 2. ������ ���̾���� ����
    for (UINT i = 0; i < MAX_LAYER; ++i)
    {
        CLayer* pLayer = _Level->GetLayer(i);

        // 2-1. ���̾� �̸� ����
        SaveWString(pLayer->GetName(), pFile);

        // 2-2. ���̾� ���� ���ӿ�����Ʈ ���� ����
        const vector<CGameObject*>& vecParent = pLayer->GetParentObject();
        size_t objCount = vecParent.size();
        fwrite(&objCount, sizeof(size_t), 1, pFile);

        // 2-3. ������Ʈ ������ŭ �ݺ����� ���� �� ���ӿ�����Ʈ ����
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
    // 1. �̸�
    SaveWString(_Object->GetName(), _File);

    // 2. ���� ������Ʈ
    for (UINT i = 0; i <= (UINT)COMPONENT_TYPE::END; ++i)
    {
        // ������Ʈ�� ���� �˱� ���� END���� �����Ѵ� -> �� ���� ������ ���̶� �ľ��ϰ� �ε� ����
        if (i == (UINT)COMPONENT_TYPE::END)
        {
            // ������Ʈ Ÿ�� ����
            fwrite(&i, sizeof(UINT), 1, _File);
            break;
        }

        CComponent* Com = _Object->GetComponent((COMPONENT_TYPE)i);
        if (nullptr == Com)
            continue;

        // 2-1. ������Ʈ Ÿ�� ����
        fwrite(&i, sizeof(UINT), 1, _File);

        // 2-2. ������Ʈ ���� ����(�� ������Ʈ���� ����)
        Com->SaveToLevelFile(_File);
    }

    // 3. ��ũ��Ʈ	

    // 3-1. ������Ʈ�� ���� ��ũ��Ʈ�� �� ���� ����
    const vector<CScript*>& vecScript = _Object->GetScripts();
    size_t ScriptCount = vecScript.size();
    fwrite(&ScriptCount, sizeof(size_t), 1, _File);

    // 3-2. ��ũ��Ʈ ������ŭ �ݺ����� ���� ���� ��ũ��Ʈ�� ������ Save ȣ��
    for (size_t i = 0; i < vecScript.size(); ++i)
    {
        // ��ũ��Ʈ �̸� ����
        wstring ScriptName = CScriptManager::GetScriptName(vecScript[i]);
        SaveWString(ScriptName, _File);

        // ��ũ��Ʈ ������ Save�Լ� ȣ��
        vecScript[i]->SaveToLevelFile(_File);
    }


    // 4. �ڽ� ������Ʈ

    // 4-1. �ڽ� �� ����
    const vector<CGameObject*>& vecChild = _Object->GetChild();
    size_t ChildCount = vecChild.size();
    fwrite(&ChildCount, sizeof(size_t), 1, _File);

    // 4-2. �ڽ� ����ŭ �ݺ��� ���� ���ȣ��
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
    // �Է����� ���� ��ο� ������ ��θ� �ٿ� �ҷ��� ������ ��� �ϼ���Ű��
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
    // �Է����� ���� ��ο� ������ ��θ� �ٿ� �ҷ��� ������ ��� �ϼ���Ű��
    wstring strPath = CPathManager::GetInst()->GetContentPath();
    strPath += _LevelPath;

    // ���� �б� ���� ����
    FILE* pFile = nullptr;
    _wfopen_s(&pFile, strPath.c_str(), L"rb");
    if (nullptr == pFile)
        return nullptr;

    // 0. ���� ����
    CLevel* NewLevel = new CLevel;

    // 1. ���� �̸� �ҷ��� �� ����
    wstring strLevelName;
    LoadWString(strLevelName, pFile);
    NewLevel->SetName(strLevelName);

    // 2. ���� �ҷ�����
    for (UINT i = 0; i < MAX_LAYER; ++i)
    {
        // 2-1. ���̾� �̸� �ҷ��� �� ����
        CLayer* pLayer = NewLevel->GetLayer(i);
        wstring LayerName;
        LoadWString(LayerName, pFile);
        pLayer->SetName(LayerName);

        // 2-2. ���̾� ���� ������Ʈ ���� �ҷ�����
        size_t objCount = 0;
        fread(&objCount, sizeof(size_t), 1, pFile);

        // 2-3. ���̾� �� ������Ʈ ������ŭ �ݺ��� ���� ������Ʈ �ҷ�����
        for (size_t j = 0; j < objCount; ++j)
        {
            CGameObject* pNewObj = LoadGameObject(pFile);
            NewLevel->AddGameObject(pNewObj, i, false);
        }
    }

    fclose(pFile);

    // �ҷ��� ������ ���¸� Stop���� ��ȯ
    NewLevel->ChangeState(LEVEL_STATE::STOP);

    return NewLevel;
}

CGameObject* CLevelSaveLoad::LoadGameObject(FILE* _File)
{
    CGameObject* pObject = new CGameObject;

    // 1. ������Ʈ �̸�
    wstring Name;
    LoadWString(Name, _File);
    pObject->SetName(Name);

    // 2. ���� ������Ʈ
    while (true)
    {
        UINT ComponentType = 0;
        fread(&ComponentType, sizeof(UINT), 1, _File);

        // ������Ʈ ������ ��(END)�� Ȯ���ϸ� while�� Ż��
        if ((UINT)COMPONENT_TYPE::END == ComponentType)
            break;

        CComponent* Component = nullptr;

        // �о�� ������Ʈ Ÿ��(����)�� ���� ������Ʈ�� �����Ҵ��Ѵ�.
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

        // �� ������Ʈ���� �ҷ��� ������ �ε�
        Component->LoadFromLevelFile(_File);

        // ������Ʈ�� �߰�
        pObject->AddComponent(Component);
    }

    // 3. ���� ��ũ��Ʈ	

    // 3-1. ��ũ��Ʈ ���� �ҷ�����
    size_t ScriptCount = 0;
    fread(&ScriptCount, sizeof(size_t), 1, _File);

    // 3-2. ������ ��ũ��Ʈ ������ŭ �ݺ��� ���� ��ũ��Ʈ �ҷ�����
    for (size_t i = 0; i < ScriptCount; ++i)
    {
        // ��ũ��Ʈ �̸� �ҷ��� ��, ��ũ��Ʈ�Ŵ������� �ش� �̸����� ��ũ��Ʈ �����Ҵ�
        wstring ScriptName;
        LoadWString(ScriptName, _File);
        CScript* pScript = CScriptManager::GetScript(ScriptName);

        // ������Ʈ�� ��ũ��Ʈ �߰� �� ��ũ��Ʈ ������ ������ �ҷ�����
        pObject->AddComponent(pScript);
        pScript->LoadFromLevelFile(_File);
    }

    // 4. �ڽ� ������Ʈ

    // 4-1. �ڽ� ���� �ҷ�����
    size_t ChildCount = 0;
    fread(&ChildCount, sizeof(size_t), 1, _File);

    // 4-2. ������ �ڽ� ������ŭ �ݺ��� ���� LoadGameObj ���ȣ��
    for (size_t i = 0; i < ChildCount; ++i)
    {
        // Load the layer index for child objects
        int ChildLayerIdx = 0;
        fread(&ChildLayerIdx, sizeof(int), 1, _File);

        CGameObject* ChildObject = LoadGameObject(_File);

        // �ڽİ� �θ� ����
        pObject->AddChild(ChildObject);
        // �ڽ� ���̾� ����
        ChildObject->ChangeLayer(ChildLayerIdx);
    }

    return pObject;
}

Ptr<CPrefab> CLevelSaveLoad::LoadPrefab(const wstring& _prefabPath)
{
    // ���ҽ��� �߰��ϱ� �� ���� Ű������ ���� ���ҽ��� �ִ��� �˻��Ѵ�(�ߺ�üũ)
    Ptr<CPrefab> pRes = CResourceManager::GetInst()->FindRes<CPrefab>(_prefabPath).Get();

    // �ش� Ű������ �̹� ����Ǿ��ִ� ���ҽ��� �ִٸ�, �װ� ��ȯ�Ѵ�.
    if (nullptr != pRes)
        return (CPrefab*)pRes.Get();

    // �Է����� ���� ��ο� ������ ��θ� �ٿ� �ҷ��� ������ ��� �ϼ���Ű��
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
