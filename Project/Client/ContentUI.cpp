#include "pch.h"
#include "ContentUI.h"

#include <Engine\CResourceManager.h>
#include <Engine\CPathManager.h>
#include <Engine\CEventManager.h>

#include "TreeUI.h"
#include "ImGuiManager.h"
#include "InspectorUI.h"
#include <Script\CLevelSaveLoad.h>

ContentUI::ContentUI()
    : UI("##Content")
{
    SetName("ContentUI");

    // ContentUI �ȿ� �ڽ����� Tree �� �߰��Ѵ�.
    m_Tree = new TreeUI;
    m_Tree->SetName("ContentTree");
    m_Tree->SetActive(true);
    m_Tree->ShowRoot(false);

    // �������� ����� m_Tree�� ��ü�� Contents(this)�̰�, �Լ������Ͱ� SetTarget..�� ��������Ʈ ����
    m_Tree->AddDynamic_Select(this, (UI_DELEGATE_1)&ContentUI::SetTargetToInspector);
    m_Tree->SetDragDropID("Resource");

    AddChildUI(m_Tree);

}

ContentUI::~ContentUI()
{
}

void ContentUI::init()
{
    Reload();
}

void ContentUI::tick()
{
    // �θ� tick ���� ����
    UI::tick();

    // ���ҽ��Ŵ������� ���� ������ ������ ����
    if (CResourceManager::GetInst()->IsResourceChanged())
    {
        ResetContent();
    }
}

int ContentUI::render_update()
{
    return 0;
}

void ContentUI::Reload()
{
    // Content ���� ���ο� �ִ� ��� ���ҽ� ���ϵ��� ��θ� ã�´�.
    m_vecResPath.clear();
    wstring strContentPath = CPathManager::GetInst()->GetContentPath();
    FindFileName(strContentPath);

    // ���ϸ� ���Ե� Ȯ���ڷ� ���ҽ��� Ÿ���� �����Ͽ� ���ҽ��� �ε��Ѵ�.
    for (size_t i = 0; i < m_vecResPath.size(); ++i)
    {
        RES_TYPE type = GetResTypeByExt(m_vecResPath[i]);

        // Ȯ���� ���߰� �ȵǴ� ������ ��� �ε����� �ʰ� continue
        if (type == RES_TYPE::END)
            continue;

        switch (type)
        {
        case RES_TYPE::MESHDATA:

            break;
        case RES_TYPE::MATERIAL:
            CResourceManager::GetInst()->Load<CMaterial>(m_vecResPath[i], m_vecResPath[i]);
            break;
        case RES_TYPE::PREFAB:
            CLevelSaveLoad::LoadPrefab(m_vecResPath[i]);
            break;
        case RES_TYPE::MESH:

            break;
        case RES_TYPE::TEXTURE:
            CResourceManager::GetInst()->Load<CTexture>(m_vecResPath[i], m_vecResPath[i]);
            break;
        case RES_TYPE::SOUND:
            CResourceManager::GetInst()->Load<CSound>(m_vecResPath[i], m_vecResPath[i]);
            break;
        }
    }

    // ���ҽ��� ���� ������ ������ �����ϴ��� üũ
    for (UINT i = 0; i < UINT(RES_TYPE::END); ++i)
    {
        const map<wstring, Ptr<CResource>>& mapRes = CResourceManager::GetInst()->GetResources((RES_TYPE)i);

        for (const auto& pair : mapRes)
        {
            // �������ҽ��� ��� Content ������ ���°� �翬�ϹǷ� �˻����� �ʰ� Skip
            if (pair.second->IsEngineRes())
                continue;

            // Ű���� �� ������̹Ƿ� ���������+����η� ��ü��θ� ���� �� ���� ���翩�� �˻�
            wstring strFilePath = strContentPath + pair.first;
            if (!filesystem::exists(strFilePath))
            {
                // �ش� ��ο� ������ �������� ���� �� ����/����� ���ҽ��̹Ƿ� 
                // ���� ���ҽ� �ʿ� �ε��صξ��� ���ҽ��� �����ؾ��Ѵ�.
                // �̺�Ʈ�Ŵ����� ó���Ѵ�

                tEvent evn = {};
                evn.Type = EVENT_TYPE::DELETE_RESOURCE;
                evn.wParam = (DWORD_PTR)i;
                evn.lParam = (DWORD_PTR)pair.second.Get();
                CEventManager::GetInst()->AddEvent(evn);
            }
        }
    }
    // ���ҽ� �ε��� �Ϸ��ߴٸ� ������UI ������ �����ش�.
    ResetContent();
}

void ContentUI::ResetContent()
{
    m_Tree->Clear();
 
    //�ֻ��� �θ� ������ �� �� ��Ʈ�� �ִ´�. ���� �ʴ´ٸ� ���� ó���� ���� ���ҽ��� �ֻ��� ��Ʈ�� �� ��
    m_Tree->AddItem("Root", 0);

    // ���ҽ� �Ŵ������� ���� ��� ���ҽ� ��� �޾ƿ�
    for (size_t i = 0; i < (UINT)RES_TYPE::END; ++i)
    {
        const map<wstring, Ptr<CResource>>& mapRes = CResourceManager::GetInst()->GetResources((RES_TYPE)i);

        // m_Tree �� ���� ���ҽ� ����� AddItem
        TreeNode* pCategory = m_Tree->AddItem(ToString((RES_TYPE)i), 0);
        pCategory->SetCategoryNode(true);

        for (const auto& pair : mapRes)
        {
            m_Tree->AddItem(string(pair.first.begin(), pair.first.end()), (DWORD_PTR)pair.second.Get(), pCategory);
        }
    }
}

void ContentUI::SetTargetToInspector(DWORD_PTR _SelectedNode)
{
    TreeNode* pSelectedNode = (TreeNode*)_SelectedNode;
    CResource* pSelectObject = (CResource*)pSelectedNode->GetData();

    if (nullptr == pSelectObject)
        return;

    // Inspector �� ���õ� Resource �� �˷��ش�.	
    InspectorUI* pInspector = (InspectorUI*)ImGuiManager::GetInst()->FindUI("##Inspector");
    pInspector->SetTargetResource(pSelectObject);
}

void ContentUI::FindFileName(const wstring& _FolderPath)
{
    // ������ ����(Ʈ�� ����)�� ��ȸ�Ͽ� �ش� ���� ���� ��� ������ ã�ƿ��� �Լ�

    // L"*.*" : � �̸�, � Ȯ�������� ��� ���� ��� �˻��ϰڴٴ� �ǹ� 
    wstring FolderPath = _FolderPath + L"*.*";

    WIN32_FIND_DATA FindData = {};

    // �ش� ��ο� �ִ� ���� ù ������ ã�ƿ´�. ��ȯ���� ���� Ž�� ����� ���� Ž���� �ڵ��̴�.
    HANDLE hFindHandle = FindFirstFile(FolderPath.c_str(), &FindData);

    // �� �Լ��� ��ȯ���� �ڵ��, ���� �����Ͱ� ������ ������ ���� ������ ã�ƿ´�.
    while (FindNextFile(hFindHandle, &FindData))
    {
        // ã�ƿ� ������ ����(���丮) Ÿ���̶��
        if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            // �������� �� ..����(Ŭ�� �� ���������� ���� ����� ����)�� ã�Ҵٸ� -> Continue
            if (!wcscmp(FindData.cFileName, L".."))
            {
                continue;
            }

            // �ش� ���������� �Լ��� ���ȣ���Ͽ� ���ο� �ڵ��� ����� �ش� ���� ���� ���� ��� ã�ƿ���
            FindFileName(_FolderPath + FindData.cFileName + L"\\");
            continue;
        }

        // ã�ƿ� ����ο��� Content ���������� ��θ� ���� Content ���� �������� ��θ� ���� ��, ���Ϳ� �߰��Ѵ�.
        wstring strContentPath = CPathManager::GetInst()->GetContentPath();
        wstring RelativePath = _FolderPath + FindData.cFileName;
        RelativePath = RelativePath.substr(strContentPath.length(), RelativePath.length() - strContentPath.length());

        m_vecResPath.push_back(RelativePath);
    }

    // Ž����� �ڵ�(Ŀ�� ������Ʈ)�̹Ƿ� ��� �� �ݵ�� Close���ش�.
    FindClose(hFindHandle);
}

RES_TYPE ContentUI::GetResTypeByExt(const wstring& _relativepath)
{
    // ���ҽ� ������ ����ο��� Ȯ���ڸ��� ������ �� �ش� Ȯ���ڷ� ���ҽ� Ÿ���� �����Ѵ�.
    wchar_t szExt[50] = {};
    _wsplitpath_s(_relativepath.c_str(), 0, 0, 0, 0, 0, 0, szExt, 50);
    wstring strExt = szExt;

    if (L".mdat" == strExt)
        return RES_TYPE::MESHDATA;

    else if (L".pref" == strExt)
        return RES_TYPE::PREFAB;

    else if (L".mesh" == strExt)
        return RES_TYPE::MESH;

    else if (L".mtrl" == strExt)
        return RES_TYPE::MATERIAL;

    else if (L".png" == strExt || L".jpg" == strExt
        || L".jpeg" == strExt || L".bmp" == strExt
        || L".tga" == strExt || L".dds" == strExt)
        return RES_TYPE::TEXTURE;

    else if (L".mp3" == strExt || L".wav" == strExt || L".ogg" == strExt)
        return RES_TYPE::SOUND;

    else
        return RES_TYPE::END;
}
