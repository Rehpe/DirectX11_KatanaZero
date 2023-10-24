#pragma once
#include "UI.h"

// =========
// TreeNode
// =========

class TreeUI;

class TreeNode
{
public:
    TreeNode();
    ~TreeNode();

friend class TreeUI;

private:
    TreeUI*             m_Owner;        // 노드를 소유하고 있는 트리
    TreeNode*           m_ParentNode;   // 부모노드
    vector<TreeNode*>   m_vecChildNode; // 노드의 자식 노드

    string              m_strName;      // 노드의 출력 이름
    UINT                m_ID;           // 노드의 고유 ID

    string              m_Rename;

    DWORD_PTR           m_Data;         // 노드에 저장된 데이터

    bool                m_CategoryNode;   // 항목 대표 노드
    bool                m_Highlight;      // 노드 하이라이트 처리
    bool                m_DoubleClicked;

private:
    void render_update();

public:
    const string& GetName() { return m_strName; }
    const string& GetRename() { return m_Rename; }
    DWORD_PTR GetData() { return m_Data; }

public:
    void SetCategoryNode(bool _category)
    {
        m_CategoryNode = _category;
    }

    void SetDoubleClicked(bool _doubleclick)
    {
        m_DoubleClicked = _doubleclick;
    }

};


// ======
// TreeUI
// ======

class TreeUI :
    public UI
{
public:
    TreeUI();
    ~TreeUI();

    friend class TreeNode;

private:
    TreeNode*       m_RootNode; // 트리가 소유하고 있는 노드 중 루트 노드
    UINT            g_NextId;   // 생성되는 노드뒤에 붙여줄 고유 숫자

    bool            m_bShowRoot;

    TreeNode*       m_SelectedNode;     // 선택된 노드
    TreeNode*       m_LbtDownNode;      // 위에서 L버튼이 눌린 노드

    TreeNode*       m_DragNode;
    TreeNode*       m_DropNode;

    DWORD_PTR       m_dwPrevSelected;     // 트리가 재구성되기 전 선택하고 있던 노드

    UI*             m_SelectInst;
    UI_DELEGATE_1   m_SelectFunc;         // 트리의 노드가 선택됐을 때 트리에서 호출할 함수와 객체(델리게이트)

    UI*             m_DragDropInst;
    UI_DELEGATE_2   m_DragDropFunc;       // 트리에서 드래그드롭이 발생했을 때 트리에서 호출할 함수와 객체

    UI*             m_DoubleClickInst;
    UI_DELEGATE_1   m_DoubleClickFunc;

    string          m_strDragDropID;

public:
    virtual int render_update() override;
    void Clear();

public:
    TreeNode* AddItem(const string& _strNodeName, DWORD_PTR _Data, TreeNode* _pParent = nullptr);
    void ShowRoot(bool _Show) { m_bShowRoot = _Show; }

    // 트리 내의 노드가 선택됐을 경우 실행할 함수(아웃라이너, 콘텐츠 등 트리를 사용하는 주체에 따라 다르게 설정 가능)
    void AddDynamic_Select(UI* _UI, UI_DELEGATE_1 _MemFunc)
    {
        m_SelectInst = _UI;
        m_SelectFunc = _MemFunc;
    }

    // 트리에서 드래그드롭이 발생했을 경우 실행할 함수(
    // ** 아웃라이너의 경우: 오브젝트 계층구조 변경
    // ** 리소스의 경우: PayLoad
    void AddDynamic_DragDrop(UI* _UI, UI_DELEGATE_2 _MemFunc)
    {
        m_DragDropInst = _UI;
        m_DragDropFunc = _MemFunc;
    }

    void AddDynamic_DoubleClick(UI* _UI, UI_DELEGATE_1 _MemFunc)
    {
        m_DoubleClickInst = _UI;
        m_DoubleClickFunc = _MemFunc;
    }


    // 드래그드롭을 위한 PayLoad ID 설정 함수
    void SetDragDropID(const string& _strID) { m_strDragDropID = _strID; }

private:
    void SetSelectedNode(TreeNode* _Node);
    void SetDragNode(TreeNode* _Node);
    void SetDropNode(TreeNode* _Node);

    void DoubleClickNode(TreeNode* _Node);

public:
    TreeNode* GetSelectedNode() { return m_SelectedNode; }
    bool GetSelectedNode(DWORD_PTR _Data);  // 입력된 데이터와 동일한 데이터를 보유하고 있는 노드를 선택상태로 만듦



};

