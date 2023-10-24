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

	// OutlinerUI 안에 자식으로 Tree 를 추가한다.
	m_Tree = new TreeUI;
	m_Tree->SetName("OutlinerTree");
	m_Tree->SetActive(true);
	m_Tree->ShowRoot(false);
	
	// 아웃라이너의 Tree에 객체가 Outliner(this)이고, 함수포인터가 SetTarget..인 델리게이트 설정
	m_Tree->AddDynamic_Select(this, (UI_DELEGATE_1)&OutlinerUI::SetTargetToInspector);
	// 아웃라이너의 Tree에 객체가 Outliner(this)이고, 함수포인터가 DragDrop인 델리게이트 설정
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

		// 트리 리셋 후, 기존 선택노드 데이터(게임오브젝트)가 있으면, 찾아서 강제로 선택노드로 변경
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

	// 레벨매니저에서 현재 모든 최상위부모 게임오브젝트 목록 받아옴
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
	// 아웃라이너의 m_Tree의 Node가 클릭됐을 때 m_Tree가 아웃라이너에게 호출시킬 함수
	// 선택된 노드의 게임오브젝트 데이터를 InspectorUI에게 전달하는(InspectorUI의 target을 노드의 데이터로 교체하는) 함수

	// 선택된 노드가 가지고 있는 게임 데이터 주소 가져오기
	TreeNode* pSelectedNode = (TreeNode*)_SelectedNode;
	CGameObject* pSelectObject = (CGameObject*)pSelectedNode->GetData();
	
	if (pSelectObject == nullptr)
		return;

	// Inspector 에 선택된 GameObject 를 알려준다.	
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
	// 오브젝트를 트리에 넣고, 생성된 노드 주소를 받아둔다.
	TreeNode* pNode = m_Tree->AddItem(string(_Obj->GetName().begin(), _Obj->GetName().end())
		, (DWORD_PTR)_Obj
		, _ParentNode);

	// 오브젝트의 자식오브젝트들을 오브젝트 노드를 부모로 해서 그 밑으로 다시 넣어준다.
	const vector<CGameObject*>& vecChild = _Obj->GetChild();
	for (size_t i = 0; i < vecChild.size(); ++i)
	{
		AddGameObject(vecChild[i], pNode);
	}
}

void OutlinerUI::DragDrop(DWORD_PTR _DragNode, DWORD_PTR _DropNode)
{
	// 1. 인자로 들어온 드래그/드랍 노드 TreeNode*로 변환
	TreeNode* pDragNode = (TreeNode*)_DragNode;
	TreeNode* pDropNode = (TreeNode*)_DropNode;

	// 2. 인자로 들어온 노드의 데이터(게임오브젝트) 가져오기
	CGameObject* pDragObj = (CGameObject*)pDragNode->GetData();
	CGameObject* pDropObj = nullptr;
	if (nullptr != pDropNode)
	{
		pDropObj = (CGameObject*)pDropNode->GetData();
	}

	// 자식으로 들어갈 오브젝트가 목적지 오브젝트의 조상(부모계층) 중 하나라면, 
	// AddChild를 아예 실행시키지 않는다
	if (nullptr != pDropObj)
	{
		if (pDropObj->IsAncestor(pDragObj))
			return;
	}

	// 이벤트 매니저를 통해서 처리한다.
	tEvent evn = {};
	evn.Type = EVENT_TYPE::ADD_CHILD;
	evn.wParam = (DWORD_PTR)pDropObj;
	evn.lParam = (DWORD_PTR)pDragObj;
	CEventManager::GetInst()->AddEvent(evn);
}

void OutlinerUI::RenameObject(DWORD_PTR _SelectedNode)
{
	// 아웃라이너의 m_Tree의 Node가 더블클릭됐을 때 m_Tree가 아웃라이너에게 호출시킬 함수
	// 선택된 노드의 게임오브젝트의 이름을 변경하는 함수

	// 선택된 노드가 가지고 있는 게임 데이터 주소 가져오기
	TreeNode* pSelectedNode = (TreeNode*)_SelectedNode;
	CGameObject* pSelectObject = (CGameObject*)pSelectedNode->GetData();

	// 해당 오브젝트의 이름을 변경한다.
	wstring NewName = wstring(pSelectedNode->GetRename().begin(), pSelectedNode->GetRename().end());
	pSelectObject->SetName(NewName);

	pSelectedNode->SetDoubleClicked(false);
	ResetOutliner();
}

void OutlinerUI::DeleteObject(DWORD_PTR _SelectedNode)
{
	// 아웃라이너의 m_Tree의 Node가 더블클릭됐을 때 m_Tree가 아웃라이너에게 호출시킬 함수
	// 선택된 노드의 게임오브젝트의 이름을 변경하는 함수

	// 선택된 노드가 가지고 있는 게임 데이터 주소 가져오기
	TreeNode* pSelectedNode = (TreeNode*)_SelectedNode;
	CGameObject* pSelectObject = (CGameObject*)pSelectedNode->GetData();

	// 이벤트 매니저를 통해서 처리한다.
	tEvent evn = {};
	evn.Type = EVENT_TYPE::DELETE_OBJECT;
	evn.wParam = (DWORD_PTR)pSelectObject;
	CEventManager::GetInst()->AddEvent(evn);

	//ResetOutliner();
}


