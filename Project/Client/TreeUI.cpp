#include "pch.h"
#include "TreeUI.h"

// ==========
// TreeNode
// ==========

TreeNode::TreeNode()
    : m_Owner(nullptr)
    , m_ParentNode(nullptr)
    , m_ID(0)
    , m_Data(0)
    , m_CategoryNode(false)
    , m_Highlight(false)
{
}

TreeNode::~TreeNode()
{
    Safe_Del_Vec(m_vecChildNode);
}

void TreeNode::render_update()
{
    // FinalName 만들기
    string strFinalName = m_strName;
    strFinalName += "##";

    char szBuff[100] = {};
    itoa(m_ID, szBuff, 10); // 본인의 숫자 ID값을 string 변환 후 붙여서 fullID 만들기
    strFinalName += szBuff;

    UINT flag = ImGuiTreeNodeFlags_DefaultOpen;

    // 자식 노드가 없으면 Leaf 플래그를 설정한다(화살표 제거)
    if (m_vecChildNode.empty())
        flag |= ImGuiTreeNodeFlags_Leaf;

    // 클릭 되었거나, 항목 대표 노드인 경우 Selected 플래그로 하이라이트를 준다.
    if (m_Highlight || m_CategoryNode)
        flag |= ImGuiTreeNodeFlags_Selected;

    if (m_DoubleClicked)
    {
        if (m_Owner->m_DoubleClickInst && m_Owner->m_DoubleClickFunc)
        {
            char newname[256] = {};
            if (ImGui::InputText(szBuff, newname, 256, (ImGuiInputTextFlags)ImGuiInputTextFlags_EnterReturnsTrue))
            {
                m_Rename = newname;
                m_Owner->DoubleClickNode(this);
                return;
            }
        }
    }

    if (ImGui::TreeNodeEx(strFinalName.c_str(), flag))
    {
        // 해당 노드에 마우스 왼클릭이 발생하면 선택&하이라이트를 준다.
        if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
        {
            m_Owner->m_LbtDownNode = this;
        }
        else if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_::ImGuiMouseButton_Left))
        {
            m_Owner->SetSelectedNode(this);
        }
        // 또는, 트리 재구성 이전에 선택된 노드였다면, 다시 선택노드로 처리한다.
        else if (0 != m_Owner->m_dwPrevSelected && m_Owner->m_dwPrevSelected == m_Data)
        {
            // 이전 선택노드(데이터)를 0으로 초기화한 뒤, 선택노드를 이전 선택노드로 변경한다.
            m_Owner->m_dwPrevSelected = 0;
            m_Owner->SetSelectedNode(this);
        }
        
        // 선택된 노드를 더블클릭 했을 경우
        if (ImGui::IsItemHovered() && m_Highlight && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
        {
            m_DoubleClicked = true;
        }

        // 해당 노드 위에서 드래그 스타트 체크
        if (ImGui::BeginDragDropSource())
        {
            // 인자: PayLoad 키값, 데이터 주소, 데이터 크기, 페이로드 조건(현재는 디폴트)
            ImGui::SetDragDropPayload(m_Owner->m_strDragDropID.c_str(), this, sizeof(TreeNode));
            ImGui::Text(m_strName.c_str());     // 드래그 시 따라다닐 텍스트(노드 데이터 이름)

            // Tree 에 드래그 노드로 등록
            m_Owner->SetDragNode(this);

            ImGui::EndDragDropSource();
        }

        // 드래그 시작 후, 드랍의 후보인 경우
        if (ImGui::BeginDragDropTarget())
        {
            // 해당 노드에서 마우스 뗀 경우 + 지정한 PayLoad 키값이 일치한 경우
            const ImGuiPayload* pPayLoad = ImGui::AcceptDragDropPayload(m_Owner->m_strDragDropID.c_str());
            if (pPayLoad)
            {
                // Tree 에 드랍 노드로 등록
                m_Owner->SetDropNode(this);
            }

            ImGui::EndDragDropTarget();
        }

        // 트리노드 본인이 가진 자식노드의 개수만큼 반복문을 돌며 자식에게 render_update 호출(재귀)
        for (size_t i = 0; i < m_vecChildNode.size(); ++i)
        {
            m_vecChildNode[i]->render_update();
        }

        ImGui::TreePop();
    }
}

// ================
//      TreeUI
// ================


TreeUI::TreeUI()
    : UI("##Tree")
    , m_RootNode(nullptr)
    , g_NextId(0)
    , m_bShowRoot(true)
    , m_SelectedNode(nullptr)
    , m_SelectInst(nullptr)
    , m_SelectFunc(nullptr)
    , m_DragDropInst(nullptr)
    , m_DragDropFunc(nullptr)
{
    m_strDragDropID = "PayLoad";
}

TreeUI::~TreeUI()
{
    if (nullptr != m_RootNode)
        delete m_RootNode;
}

int TreeUI::render_update()
{
    // 루트노드가 있다면
    if (nullptr != m_RootNode)
    {
        // 루트를 보여줄 거라면(모든 컨텐츠를 묶은 최상위 루트노드)
        if (m_bShowRoot)
        {
            m_RootNode->render_update();
        }
        else
        {
            // 최상위 루트노드 출력 없이 자식들에게 render_update 호출
            for (size_t i = 0; i < m_RootNode->m_vecChildNode.size(); ++i)
            {
                m_RootNode->m_vecChildNode[i]->render_update();
            }
        }
    }

    // 1. Drag, Drop 노드 둘 다 저장되어있는 경우 -> 드래그드롭이 일어났다
    // 2. Drag만 저장, Drop은 저장되지 않았으나 Mouse Release 발생(허공에 드래그) -> 드래그 노드를 최상위 부모로 뺀다
    if ( (m_DragNode && m_DropNode) || (m_DragNode && ImGui::IsMouseReleased(ImGuiMouseButton_::ImGuiMouseButton_Left)))
    {
        // 드래그드랍이 일어났을 경우 호출할 객체 & 함수가 등록되어있다
        if (m_DragDropInst && m_DragDropFunc)
        {   
            // 해당 객체의 함수 호출
            (m_DragDropInst->*m_DragDropFunc)((DWORD_PTR)m_DragNode, (DWORD_PTR)m_DropNode);
        }

        // 드래그/드랍 노드 초기화
        m_DragNode = nullptr;
        m_DropNode = nullptr;
    }

    return 0;
}

void TreeUI::Clear()
{
    // 이전에 선택된 노드가 있으면, 트리를 초기화하기 전 그 노드를 선택됐던 노드로 저장
    if (nullptr != m_SelectedNode)
    {
        // DWORD_PTR(8바이트 정수) 타입으로, 노드가 가진 실제 데이터를 저장
        m_dwPrevSelected = m_SelectedNode->GetData();
    }

    if (nullptr != m_RootNode)
    {
        delete m_RootNode;
        m_RootNode = nullptr;
    }
}

TreeNode* TreeUI::AddItem(const string& _strNodeName, DWORD_PTR _Data, TreeNode* _pParent)
{
    TreeNode* pNewNode = new TreeNode;

    pNewNode->m_Owner = this;
    pNewNode->m_strName = _strNodeName;
    pNewNode->m_Data = _Data;
    pNewNode->m_ID = g_NextId++;

    // 루트노드가 없다 -> 트리에 들어온 최초의 데이터
    if (nullptr == m_RootNode)
    {
        // 최초 데이터 입력인데, 부모를 지정한 경우 -> 에러!
        assert(!_pParent);

        // 트리에 들어온 첫 데이터는 무조건 루트노드로 등록된다.
        m_RootNode = pNewNode;
    }

    else
    {
        // 밑으로 들어갈 부모를 지정한 경우
        if (_pParent)
        {
            // 노드의 부모로 지정된 노드가 해당 트리 소속이 아니다 -> 에러!
            if (_pParent->m_Owner != this)
                assert(nullptr);

            // 지정된 부모의 자식으로 연결
            _pParent->m_vecChildNode.push_back(pNewNode);
            pNewNode->m_ParentNode = _pParent;
        }

        // 부모로 지정된 노드가 없는경우, 루트 밑으로 넣는다
        else
        {
            // 새로 생성한 노드를 루트노드의 자식으로 연결
            m_RootNode->m_vecChildNode.push_back(pNewNode);
            pNewNode->m_ParentNode = m_RootNode;
        }
    }

    return pNewNode;
}

void TreeUI::SetSelectedNode(TreeNode* _Node)
{
    // 노드가 선택된 경우 기존 하이라이트를 꺼주고
    if (m_SelectedNode)
        m_SelectedNode->m_Highlight = false;

    // 선택된 노드로 설정
    m_SelectedNode = _Node;

    // 하이라이트 켜주고, 함수포인터 호출
    if (m_SelectedNode)
    {
        m_SelectedNode->m_Highlight = true;

        if (m_SelectInst && m_SelectFunc)
        {
            (m_SelectInst->*m_SelectFunc)((DWORD_PTR)m_SelectedNode);
        }
    }
}

void TreeUI::SetDragNode(TreeNode* _Node)
{
    m_DragNode = _Node;
    // 이전 드롭노드에 남아있는 노드가 있을 수 있으므로 nullptr로 초기화
    m_DropNode = nullptr;
}

void TreeUI::SetDropNode(TreeNode* _Node)
{
    m_DropNode = _Node;
}

void TreeUI::DoubleClickNode(TreeNode* _Node)
{
    // 더블클릭이 일어났을 경우 호출할 객체 & 함수가 등록되어있다
    if (m_DoubleClickInst && m_DoubleClickFunc)
    {
        _Node->m_DoubleClicked = true;
        // 해당 객체의 함수 호출
        (m_DoubleClickInst->*m_DoubleClickFunc)((DWORD_PTR)_Node);
    }
}

bool TreeUI::GetSelectedNode(DWORD_PTR _Data)
{
    // 인자로 들어온 데이터와 동일한 데이터가 있는지 모든 노드를 BFS로 검색하여 찾는다
    static list<TreeNode*> queue;
    queue.clear();

    queue.push_back(m_RootNode);

    TreeNode* pCurNode = nullptr;
    while (!queue.empty())
    {
        pCurNode = queue.front();
        queue.pop_front();

        for (size_t i = 0; i < pCurNode->m_vecChildNode.size(); ++i)
        {
            queue.push_back(pCurNode->m_vecChildNode[i]);
        }

        if (pCurNode->GetData() == _Data)
        {
            SetSelectedNode(pCurNode);
            return true;
        }
    }

    return false;
}
