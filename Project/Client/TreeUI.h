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
    TreeUI*             m_Owner;        // ��带 �����ϰ� �ִ� Ʈ��
    TreeNode*           m_ParentNode;   // �θ���
    vector<TreeNode*>   m_vecChildNode; // ����� �ڽ� ���

    string              m_strName;      // ����� ��� �̸�
    UINT                m_ID;           // ����� ���� ID

    string              m_Rename;

    DWORD_PTR           m_Data;         // ��忡 ����� ������

    bool                m_CategoryNode;   // �׸� ��ǥ ���
    bool                m_Highlight;      // ��� ���̶���Ʈ ó��
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
    TreeNode*       m_RootNode; // Ʈ���� �����ϰ� �ִ� ��� �� ��Ʈ ���
    UINT            g_NextId;   // �����Ǵ� ���ڿ� �ٿ��� ���� ����

    bool            m_bShowRoot;

    TreeNode*       m_SelectedNode;     // ���õ� ���
    TreeNode*       m_LbtDownNode;      // ������ L��ư�� ���� ���

    TreeNode*       m_DragNode;
    TreeNode*       m_DropNode;

    DWORD_PTR       m_dwPrevSelected;     // Ʈ���� �籸���Ǳ� �� �����ϰ� �ִ� ���

    UI*             m_SelectInst;
    UI_DELEGATE_1   m_SelectFunc;         // Ʈ���� ��尡 ���õ��� �� Ʈ������ ȣ���� �Լ��� ��ü(��������Ʈ)

    UI*             m_DragDropInst;
    UI_DELEGATE_2   m_DragDropFunc;       // Ʈ������ �巡�׵���� �߻����� �� Ʈ������ ȣ���� �Լ��� ��ü

    UI*             m_DoubleClickInst;
    UI_DELEGATE_1   m_DoubleClickFunc;

    string          m_strDragDropID;

public:
    virtual int render_update() override;
    void Clear();

public:
    TreeNode* AddItem(const string& _strNodeName, DWORD_PTR _Data, TreeNode* _pParent = nullptr);
    void ShowRoot(bool _Show) { m_bShowRoot = _Show; }

    // Ʈ�� ���� ��尡 ���õ��� ��� ������ �Լ�(�ƿ����̳�, ������ �� Ʈ���� ����ϴ� ��ü�� ���� �ٸ��� ���� ����)
    void AddDynamic_Select(UI* _UI, UI_DELEGATE_1 _MemFunc)
    {
        m_SelectInst = _UI;
        m_SelectFunc = _MemFunc;
    }

    // Ʈ������ �巡�׵���� �߻����� ��� ������ �Լ�(
    // ** �ƿ����̳��� ���: ������Ʈ �������� ����
    // ** ���ҽ��� ���: PayLoad
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


    // �巡�׵���� ���� PayLoad ID ���� �Լ�
    void SetDragDropID(const string& _strID) { m_strDragDropID = _strID; }

private:
    void SetSelectedNode(TreeNode* _Node);
    void SetDragNode(TreeNode* _Node);
    void SetDropNode(TreeNode* _Node);

    void DoubleClickNode(TreeNode* _Node);

public:
    TreeNode* GetSelectedNode() { return m_SelectedNode; }
    bool GetSelectedNode(DWORD_PTR _Data);  // �Էµ� �����Ϳ� ������ �����͸� �����ϰ� �ִ� ��带 ���û��·� ����



};

