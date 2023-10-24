#pragma once
#include "UI.h"

class TreeUI;
class TreeNode;

class OutlinerUI :
    public UI
{
public:
    OutlinerUI();
    ~OutlinerUI();

private:
    TreeUI*         m_Tree;
    DWORD_PTR       m_dwSelectedData;  

public:
    virtual void tick() override;
    virtual int render_update() override;

public:
    void ResetOutliner();
    void SetTargetToInspector(DWORD_PTR _SelectedNode);     // Ʈ���� Ŭ������ �� �ƿ����̳ʰ� ������ �Լ�
    CGameObject* GetSelectedObject();

    // Ʈ���� ���µ� ���, �Է����� ���� �����Ϳ� ������ ��带 ���� ó���Ѵ�.
    void SetSelectedNodeData(DWORD_PTR _data)
    {
        m_dwSelectedData = _data;
    }


private:
    void AddGameObject(CGameObject* _Obj, TreeNode* _ParentNode);
    void DragDrop(DWORD_PTR _DragNode, DWORD_PTR _DropNode); // Ʈ���� �巡�׵�ӽ� �ƿ����̳ʰ� ������ �Լ�
    void RenameObject(DWORD_PTR _Node);
    void DeleteObject(DWORD_PTR _Node);
};

