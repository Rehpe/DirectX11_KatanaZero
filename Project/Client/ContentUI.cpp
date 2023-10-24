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

    // ContentUI 안에 자식으로 Tree 를 추가한다.
    m_Tree = new TreeUI;
    m_Tree->SetName("ContentTree");
    m_Tree->SetActive(true);
    m_Tree->ShowRoot(false);

    // 컨텐츠의 멤버인 m_Tree에 객체가 Contents(this)이고, 함수포인터가 SetTarget..인 델리게이트 설정
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
    // 부모 tick 먼저 실행
    UI::tick();

    // 리소스매니저에서 변경 감지시 컨텐츠 리셋
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
    // Content 폴더 내부에 있는 모든 리소스 파일들의 경로를 찾는다.
    m_vecResPath.clear();
    wstring strContentPath = CPathManager::GetInst()->GetContentPath();
    FindFileName(strContentPath);

    // 파일명에 포함된 확장자로 리소스의 타입을 유추하여 리소스를 로딩한다.
    for (size_t i = 0; i < m_vecResPath.size(); ++i)
    {
        RES_TYPE type = GetResTypeByExt(m_vecResPath[i]);

        // 확장자 유추가 안되는 파일의 경우 로드하지 않고 continue
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

    // 리소스의 원본 파일이 실제로 존재하는지 체크
    for (UINT i = 0; i < UINT(RES_TYPE::END); ++i)
    {
        const map<wstring, Ptr<CResource>>& mapRes = CResourceManager::GetInst()->GetResources((RES_TYPE)i);

        for (const auto& pair : mapRes)
        {
            // 엔진리소스의 경우 Content 폴더에 없는게 당연하므로 검사하지 않고 Skip
            if (pair.second->IsEngineRes())
                continue;

            // 키값이 곧 상대경로이므로 콘텐츠경로+상대경로로 전체경로를 구한 뒤 파일 존재여부 검사
            wstring strFilePath = strContentPath + pair.first;
            if (!filesystem::exists(strFilePath))
            {
                // 해당 경로에 파일이 존재하지 않을 시 삭제/변경된 리소스이므로 
                // 기존 리소스 맵에 로드해두었던 리소스를 삭제해야한다.
                // 이벤트매니저로 처리한다

                tEvent evn = {};
                evn.Type = EVENT_TYPE::DELETE_RESOURCE;
                evn.wParam = (DWORD_PTR)i;
                evn.lParam = (DWORD_PTR)pair.second.Get();
                CEventManager::GetInst()->AddEvent(evn);
            }
        }
    }
    // 리소스 로딩을 완료했다면 콘텐츠UI 갱신을 시켜준다.
    ResetContent();
}

void ContentUI::ResetContent()
{
    m_Tree->Clear();
 
    //최상위 부모 역할을 할 빈 루트를 넣는다. 넣지 않는다면 가장 처음에 들어온 리소스가 최상위 루트가 될 것
    m_Tree->AddItem("Root", 0);

    // 리소스 매니저에서 현재 모든 리소스 목록 받아옴
    for (size_t i = 0; i < (UINT)RES_TYPE::END; ++i)
    {
        const map<wstring, Ptr<CResource>>& mapRes = CResourceManager::GetInst()->GetResources((RES_TYPE)i);

        // m_Tree 에 현재 리소스 목록을 AddItem
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

    // Inspector 에 선택된 Resource 를 알려준다.	
    InspectorUI* pInspector = (InspectorUI*)ImGuiManager::GetInst()->FindUI("##Inspector");
    pInspector->SetTargetResource(pSelectObject);
}

void ContentUI::FindFileName(const wstring& _FolderPath)
{
    // 윈도우 폴더(트리 구조)를 순회하여 해당 폴더 밑의 모든 파일을 찾아오는 함수

    // L"*.*" : 어떤 이름, 어떤 확장자인지 상관 없이 모두 검색하겠다는 의미 
    wstring FolderPath = _FolderPath + L"*.*";

    WIN32_FIND_DATA FindData = {};

    // 해당 경로에 있는 제일 첫 파일을 찾아온다. 반환값은 파일 탐색 기능을 가진 탐색기 핸들이다.
    HANDLE hFindHandle = FindFirstFile(FolderPath.c_str(), &FindData);

    // 위 함수로 반환받은 핸들로, 다음 데이터가 존재할 때까지 다음 파일을 찾아온다.
    while (FindNextFile(hFindHandle, &FindData))
    {
        // 찾아온 파일이 폴더(디렉토리) 타입이라면
        if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            // 숨김폴더 중 ..폴더(클릭 시 상위폴더로 가는 기능을 가짐)를 찾았다면 -> Continue
            if (!wcscmp(FindData.cFileName, L".."))
            {
                continue;
            }

            // 해당 폴더명으로 함수를 재귀호출하여 새로운 핸들을 만들어 해당 폴더 내의 파일 모두 찾아오기
            FindFileName(_FolderPath + FindData.cFileName + L"\\");
            continue;
        }

        // 찾아온 상대경로에서 Content 폴더까지의 경로를 빼서 Content 폴더 밑으로의 경로만 남긴 뒤, 벡터에 추가한다.
        wstring strContentPath = CPathManager::GetInst()->GetContentPath();
        wstring RelativePath = _FolderPath + FindData.cFileName;
        RelativePath = RelativePath.substr(strContentPath.length(), RelativePath.length() - strContentPath.length());

        m_vecResPath.push_back(RelativePath);
    }

    // 탐색기는 핸들(커널 오브젝트)이므로 사용 후 반드시 Close해준다.
    FindClose(hFindHandle);
}

RES_TYPE ContentUI::GetResTypeByExt(const wstring& _relativepath)
{
    // 리소스 파일의 상대경로에서 확장자만을 추출한 뒤 해당 확장자로 리소스 타입을 유추한다.
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
