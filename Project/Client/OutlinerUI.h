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
    void SetTargetToInspector(DWORD_PTR _SelectedNode);     // 트리가 클릭됐을 때 아웃라이너가 실행할 함수
    CGameObject* GetSelectedObject();

    // 트리가 리셋된 경우, 입력으로 들어온 데이터와 동일한 노드를 선택 처리한다.
    void SetSelectedNodeData(DWORD_PTR _data)
    {
        m_dwSelectedData = _data;
    }


private:
    void AddGameObject(CGameObject* _Obj, TreeNode* _ParentNode);
    void DragDrop(DWORD_PTR _DragNode, DWORD_PTR _DropNode); // 트리의 드래그드롭시 아웃라이너가 실행할 함수
    void RenameObject(DWORD_PTR _Node);
    void DeleteObject(DWORD_PTR _Node);
};


