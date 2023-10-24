#include "pch.h"
#include "OutlinerUI.h"

#include <Engine\CEventManager.h>
#include <Engine\CLevelManager.h>
#include <Engine\CLevel.h>
#include <Engine\CLayer.h>
#include <Engine\CGameObject.h>

#include "TreeUI.h"
#include "ImGuiManager.h"
#include "InspectorUI.h"

OutlinerUI::OutlinerUI()
	: UI("##Outliner")
	, m_Tree(nullptr)
{
	SetName("Outliner");

	// OutlinerUI �ȿ� �ڽ����� Tree �� �߰��Ѵ�.
	m_Tree = new TreeUI;
	m_Tree->SetName("OutlinerTree");
	m_Tree->SetActive(true);
	m_Tree->ShowRoot(false);
	
	// �ƿ����̳��� Tree�� ��ü�� Outliner(this)�̰�, �Լ������Ͱ� SetTarget..�� ��������Ʈ ����
	m_Tree->AddDynamic_Select(this, (UI_DELEGATE_1)&OutlinerUI::SetTargetToInspector);
	// �ƿ����̳��� Tree�� ��ü�� Outliner(this)�̰�, �Լ������Ͱ� DragDrop�� ��������Ʈ ����
	m_Tree->AddDynamic_DragDrop(this, (UI_DELEGATE_2)&OutlinerUI::DragDrop);
	m_Tree->SetDragDropID("GameObject");

	m_Tree->AddDynamic_DoubleClick(this, (UI_DELEGATE_1)&OutlinerUI::RenameObject);

	AddChildUI(m_Tree);
}

OutlinerUI::~OutlinerUI()
{
}

void OutlinerUI::tick()
{
	if (CEventManager::GetInst()->IsLevelChanged())
	{
		ResetOutliner();

		// Ʈ�� ���� ��, ���� ���ó�� ������(���ӿ�����Ʈ)�� ������, ã�Ƽ� ������ ���ó��� ����
		if (0 != m_dwSelectedData)
		{
			m_Tree->GetSelectedNode(m_dwSelectedData);
		}
	}
}

int OutlinerUI::render_update()
{
	return 0;
}

void OutlinerUI::ResetOutliner()
{
	// Tree Clear
	m_Tree->Clear();
	m_Tree->AddItem("Root", 0);

	// �����Ŵ������� ���� ��� �ֻ����θ� ���ӿ�����Ʈ ��� �޾ƿ�
	CLevel* pCurLevel = CLevelManager::GetInst()->GetCurLevel();

	if (pCurLevel == nullptr)
		return;
	  
	for (UINT i = 0; i < (UINT)MAX_LAYER; ++i)
	{
		CLayer* pLayer = pCurLevel->GetLayer(i);

		const vector<CGameObject*>& vecParentObj = pLayer->GetParentObject();

		for (size_t i = 0; i < vecParentObj.size(); ++i)
		{
			if (!vecParentObj[i]->IsDead())
			{
				AddGameObject(vecParentObj[i], nullptr);
			}
		}
	}
}

void OutlinerUI::SetTargetToInspector(DWORD_PTR _SelectedNode)
{
	// �ƿ����̳��� m_Tree�� Node�� Ŭ������ �� m_Tree�� �ƿ����̳ʿ��� ȣ���ų �Լ�
	// ���õ� ����� ���ӿ�����Ʈ �����͸� InspectorUI���� �����ϴ�(InspectorUI�� target�� ����� �����ͷ� ��ü�ϴ�) �Լ�

	// ���õ� ��尡 ������ �ִ� ���� ������ �ּ� ��������
	TreeNode* pSelectedNode = (TreeNode*)_SelectedNode;
	CGameObject* pSelectObject = (CGameObject*)pSelectedNode->GetData();
	
	if (pSelectObject == nullptr)
		return;

	// Inspector �� ���õ� GameObject �� �˷��ش�.	
	InspectorUI* pInspector = (InspectorUI*)ImGuiManager::GetInst()->FindUI("##Inspector");
	pInspector->SetTargetObject(pSelectObject);
}

CGameObject* OutlinerUI::GetSelectedObject()
{
	TreeNode* pSelectedNode = m_Tree->GetSelectedNode();

	if (nullptr == pSelectedNode)
		return nullptr;

	return (CGameObject*)pSelectedNode->GetData();
}

void OutlinerUI::AddGameObject(CGameObject* _Obj, TreeNode* _ParentNode)
{
	// ������Ʈ�� Ʈ���� �ְ�, ������ ��� �ּҸ� �޾Ƶд�.
	TreeNode* pNode = m_Tree->AddItem(string(_Obj->GetName().begin(), _Obj->GetName().end())
		, (DWORD_PTR)_Obj
		, _ParentNode);

	// ������Ʈ�� �ڽĿ�����Ʈ���� ������Ʈ ��带 �θ�� �ؼ� �� ������ �ٽ� �־��ش�.
	const vector<CGameObject*>& vecChild = _Obj->GetChild();
	for (size_t i = 0; i < vecChild.size(); ++i)
	{
		AddGameObject(vecChild[i], pNode);
	}
}

void OutlinerUI::DragDrop(DWORD_PTR _DragNode, DWORD_PTR _DropNode)
{
	// 1. ���ڷ� ���� �巡��/��� ��� TreeNode*�� ��ȯ
	TreeNode* pDragNode = (TreeNode*)_DragNode;
	TreeNode* pDropNode = (TreeNode*)_DropNode;

	// 2. ���ڷ� ���� ����� ������(���ӿ�����Ʈ) ��������
	CGameObject* pDragObj = (CGameObject*)pDragNode->GetData();
	CGameObject* pDropObj = nullptr;
	if (nullptr != pDropNode)
	{
		pDropObj = (CGameObject*)pDropNode->GetData();
	}

	// �ڽ����� �� ������Ʈ�� ������ ������Ʈ�� ����(�θ����) �� �ϳ����, 
	// AddChild�� �ƿ� �����Ű�� �ʴ´�
	if (nullptr != pDropObj)
	{
		if (pDropObj->IsAncestor(pDragObj))
			return;
	}

	// �̺�Ʈ �Ŵ����� ���ؼ� ó���Ѵ�.
	tEvent evn = {};
	evn.Type = EVENT_TYPE::ADD_CHILD;
	evn.wParam = (DWORD_PTR)pDropObj;
	evn.lParam = (DWORD_PTR)pDragObj;
	CEventManager::GetInst()->AddEvent(evn);
}

void OutlinerUI::RenameObject(DWORD_PTR _SelectedNode)
{
	// �ƿ����̳��� m_Tree�� Node�� ����Ŭ������ �� m_Tree�� �ƿ����̳ʿ��� ȣ���ų �Լ�
	// ���õ� ����� ���ӿ�����Ʈ�� �̸��� �����ϴ� �Լ�

	// ���õ� ��尡 ������ �ִ� ���� ������ �ּ� ��������
	TreeNode* pSelectedNode = (TreeNode*)_SelectedNode;
	CGameObject* pSelectObject = (CGameObject*)pSelectedNode->GetData();

	// �ش� ������Ʈ�� �̸��� �����Ѵ�.
	wstring NewName = wstring(pSelectedNode->GetRename().begin(), pSelectedNode->GetRename().end());
	pSelectObject->SetName(NewName);

	pSelectedNode->SetDoubleClicked(false);
	ResetOutliner();
}

void OutlinerUI::DeleteObject(DWORD_PTR _SelectedNode)
{
	// �ƿ����̳��� m_Tree�� Node�� ����Ŭ������ �� m_Tree�� �ƿ����̳ʿ��� ȣ���ų �Լ�
	// ���õ� ����� ���ӿ�����Ʈ�� �̸��� �����ϴ� �Լ�

	// ���õ� ��尡 ������ �ִ� ���� ������ �ּ� ��������
	TreeNode* pSelectedNode = (TreeNode*)_SelectedNode;
	CGameObject* pSelectObject = (CGameObject*)pSelectedNode->GetData();

	// �̺�Ʈ �Ŵ����� ���ؼ� ó���Ѵ�.
	tEvent evn = {};
	evn.Type = EVENT_TYPE::DELETE_OBJECT;
	evn.wParam = (DWORD_PTR)pSelectObject;
	CEventManager::GetInst()->AddEvent(evn);

	//ResetOutliner();
}


