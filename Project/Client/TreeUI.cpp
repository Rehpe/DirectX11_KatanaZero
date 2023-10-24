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
    // FinalName �����
    string strFinalName = m_strName;
    strFinalName += "##";

    char szBuff[100] = {};
    itoa(m_ID, szBuff, 10); // ������ ���� ID���� string ��ȯ �� �ٿ��� fullID �����
    strFinalName += szBuff;

    UINT flag = ImGuiTreeNodeFlags_DefaultOpen;

    // �ڽ� ��尡 ������ Leaf �÷��׸� �����Ѵ�(ȭ��ǥ ����)
    if (m_vecChildNode.empty())
        flag |= ImGuiTreeNodeFlags_Leaf;

    // Ŭ�� �Ǿ��ų�, �׸� ��ǥ ����� ��� Selected �÷��׷� ���̶���Ʈ�� �ش�.
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
        // �ش� ��忡 ���콺 ��Ŭ���� �߻��ϸ� ����&���̶���Ʈ�� �ش�.
        if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
        {
            m_Owner->m_LbtDownNode = this;
        }
        else if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_::ImGuiMouseButton_Left))
        {
            m_Owner->SetSelectedNode(this);
        }
        // �Ǵ�, Ʈ�� �籸�� ������ ���õ� ��忴�ٸ�, �ٽ� ���ó��� ó���Ѵ�.
        else if (0 != m_Owner->m_dwPrevSelected && m_Owner->m_dwPrevSelected == m_Data)
        {
            // ���� ���ó��(������)�� 0���� �ʱ�ȭ�� ��, ���ó�带 ���� ���ó��� �����Ѵ�.
            m_Owner->m_dwPrevSelected = 0;
            m_Owner->SetSelectedNode(this);
        }
        
        // ���õ� ��带 ����Ŭ�� ���� ���
        if (ImGui::IsItemHovered() && m_Highlight && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
        {
            m_DoubleClicked = true;
        }

        // �ش� ��� ������ �巡�� ��ŸƮ üũ
        if (ImGui::BeginDragDropSource())
        {
            // ����: PayLoad Ű��, ������ �ּ�, ������ ũ��, ���̷ε� ����(����� ����Ʈ)
            ImGui::SetDragDropPayload(m_Owner->m_strDragDropID.c_str(), this, sizeof(TreeNode));
            ImGui::Text(m_strName.c_str());     // �巡�� �� ����ٴ� �ؽ�Ʈ(��� ������ �̸�)

            // Tree �� �巡�� ���� ���
            m_Owner->SetDragNode(this);

            ImGui::EndDragDropSource();
        }

        // �巡�� ���� ��, ����� �ĺ��� ���
        if (ImGui::BeginDragDropTarget())
        {
            // �ش� ��忡�� ���콺 �� ��� + ������ PayLoad Ű���� ��ġ�� ���
            const ImGuiPayload* pPayLoad = ImGui::AcceptDragDropPayload(m_Owner->m_strDragDropID.c_str());
            if (pPayLoad)
            {
                // Tree �� ��� ���� ���
                m_Owner->SetDropNode(this);
            }

            ImGui::EndDragDropTarget();
        }

        // Ʈ����� ������ ���� �ڽĳ���� ������ŭ �ݺ����� ���� �ڽĿ��� render_update ȣ��(���)
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
    // ��Ʈ��尡 �ִٸ�
    if (nullptr != m_RootNode)
    {
        // ��Ʈ�� ������ �Ŷ��(��� �������� ���� �ֻ��� ��Ʈ���)
        if (m_bShowRoot)
        {
            m_RootNode->render_update();
        }
        else
        {
            // �ֻ��� ��Ʈ��� ��� ���� �ڽĵ鿡�� render_update ȣ��
            for (size_t i = 0; i < m_RootNode->m_vecChildNode.size(); ++i)
            {
                m_RootNode->m_vecChildNode[i]->render_update();
            }
        }
    }

    // 1. Drag, Drop ��� �� �� ����Ǿ��ִ� ��� -> �巡�׵���� �Ͼ��
    // 2. Drag�� ����, Drop�� ������� �ʾ����� Mouse Release �߻�(����� �巡��) -> �巡�� ��带 �ֻ��� �θ�� ����
    if ( (m_DragNode && m_DropNode) || (m_DragNode && ImGui::IsMouseReleased(ImGuiMouseButton_::ImGuiMouseButton_Left)))
    {
        // �巡�׵���� �Ͼ�� ��� ȣ���� ��ü & �Լ��� ��ϵǾ��ִ�
        if (m_DragDropInst && m_DragDropFunc)
        {   
            // �ش� ��ü�� �Լ� ȣ��
            (m_DragDropInst->*m_DragDropFunc)((DWORD_PTR)m_DragNode, (DWORD_PTR)m_DropNode);
        }

        // �巡��/��� ��� �ʱ�ȭ
        m_DragNode = nullptr;
        m_DropNode = nullptr;
    }

    return 0;
}

void TreeUI::Clear()
{
    // ������ ���õ� ��尡 ������, Ʈ���� �ʱ�ȭ�ϱ� �� �� ��带 ���õƴ� ���� ����
    if (nullptr != m_SelectedNode)
    {
        // DWORD_PTR(8����Ʈ ����) Ÿ������, ��尡 ���� ���� �����͸� ����
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

    // ��Ʈ��尡 ���� -> Ʈ���� ���� ������ ������
    if (nullptr == m_RootNode)
    {
        // ���� ������ �Է��ε�, �θ� ������ ��� -> ����!
        assert(!_pParent);

        // Ʈ���� ���� ù �����ʹ� ������ ��Ʈ���� ��ϵȴ�.
        m_RootNode = pNewNode;
    }

    else
    {
        // ������ �� �θ� ������ ���
        if (_pParent)
        {
            // ����� �θ�� ������ ��尡 �ش� Ʈ�� �Ҽ��� �ƴϴ� -> ����!
            if (_pParent->m_Owner != this)
                assert(nullptr);

            // ������ �θ��� �ڽ����� ����
            _pParent->m_vecChildNode.push_back(pNewNode);
            pNewNode->m_ParentNode = _pParent;
        }

        // �θ�� ������ ��尡 ���°��, ��Ʈ ������ �ִ´�
        else
        {
            // ���� ������ ��带 ��Ʈ����� �ڽ����� ����
            m_RootNode->m_vecChildNode.push_back(pNewNode);
            pNewNode->m_ParentNode = m_RootNode;
        }
    }

    return pNewNode;
}

void TreeUI::SetSelectedNode(TreeNode* _Node)
{
    // ��尡 ���õ� ��� ���� ���̶���Ʈ�� ���ְ�
    if (m_SelectedNode)
        m_SelectedNode->m_Highlight = false;

    // ���õ� ���� ����
    m_SelectedNode = _Node;

    // ���̶���Ʈ ���ְ�, �Լ������� ȣ��
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
    // ���� ��ӳ�忡 �����ִ� ��尡 ���� �� �����Ƿ� nullptr�� �ʱ�ȭ
    m_DropNode = nullptr;
}

void TreeUI::SetDropNode(TreeNode* _Node)
{
    m_DropNode = _Node;
}

void TreeUI::DoubleClickNode(TreeNode* _Node)
{
    // ����Ŭ���� �Ͼ�� ��� ȣ���� ��ü & �Լ��� ��ϵǾ��ִ�
    if (m_DoubleClickInst && m_DoubleClickFunc)
    {
        _Node->m_DoubleClicked = true;
        // �ش� ��ü�� �Լ� ȣ��
        (m_DoubleClickInst->*m_DoubleClickFunc)((DWORD_PTR)_Node);
    }
}

bool TreeUI::GetSelectedNode(DWORD_PTR _Data)
{
    // ���ڷ� ���� �����Ϳ� ������ �����Ͱ� �ִ��� ��� ��带 BFS�� �˻��Ͽ� ã�´�
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
