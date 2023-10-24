#include "pch.h"
#include "CLayer.h"

#include "CGameObject.h"

CLayer::CLayer()
	: m_iLayerIdx(-1)
{
}

CLayer::~CLayer()
{
	Safe_Del_Vec(m_vecObject);
}

// begin, tick, finaltick은 일괄적으로 최상위 부모 게임 오브젝트에만 호출한다.
// CGameObject 내에서 내부적으로 자식벡터를 돌며 재귀호출하므로 부모->자식 순으로 올바르게 호출된다

void CLayer::begin()
{
	for (size_t i = 0; i < m_vecParentObj.size(); ++i)
	{
		m_vecParentObj[i]->begin();
	}
}

void CLayer::tick()
{
	for (size_t i = 0; i < m_vecParentObj.size(); i++)
	{
		m_vecParentObj[i]->tick();
	}
}

void CLayer::finaltick()
{
	// Dead 상태인 부모 오브젝트는 벡터에서 finaltick 호출 후 제거한다.
	// 부모가 제거된다는 말은 그 밑의 모든 자식들도 함께 제거된다는 뜻.

	vector<CGameObject*>::iterator iter = m_vecParentObj.begin();
	for (; iter != m_vecParentObj.end(); )
	{
		(*iter)->finaltick();
		
		// 모든 오브젝트의 등록은 finaltick에서 일어난다.
		// Dead 상태인 최상위 부모 오브젝트는 그 프레임의 finaltick을 마치고 삭제되어야
		// 충돌 등의 과정에서 문제가 생기지 않는다.
		if ((*iter)->IsDead())
		{
			iter = m_vecParentObj.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void CLayer::AddGameObject(CGameObject* _Object, bool _bMove)
{
	// 부모 벡터에 저장한다.
	m_vecParentObj.push_back(_Object);

	// 해당 게임 오브젝트에게 자식 오브젝트가 있는지 queue를 통해 순회하며 알아낸다.
	static list<CGameObject*> queue;
	queue.clear();

	queue.push_back(_Object);

	while (!queue.empty())	// 큐가 빌 때까지
	{
		CGameObject* pObject = queue.front();	// 맨 앞의 오브젝트를 꺼내옴
		queue.pop_front();						// 해당 오브젝트 큐에서 제거

		for (size_t i = 0; i < pObject->m_vecChild.size(); i++)
		{
			queue.push_back(pObject->m_vecChild[i]);		// 모든 자식을 큐에 저장
		}

		// 꺼내온 오브젝트가 최상위 부모거나, 레이어 미지정 상태이거나, 부모와 같이 이동 옵션(Move)이 true인 경우
		// 부모가 등록하고자 했던 레이어와 똑같은 레이어 번호를 지정한다
		if (nullptr == pObject->m_Parent ||				// 오브젝트가 최상위 부모
				 -1 == pObject->m_iLayerIdx ||			// 오브젝트가 레이어 미지정 상태(디폴트값 -1)
				_bMove)									// 부모의 레이어 따라 이동 옵션 true
			pObject->m_iLayerIdx = m_iLayerIdx;
	}
}

void CLayer::RemoveFromParentList(CGameObject* _Obj)
{
	// 부모 벡터에서 인자로 들어온 게임 오브젝트를 제거한다.
	vector<CGameObject*>::iterator iter = m_vecParentObj.begin();
	for (; iter != m_vecParentObj.end(); ++iter)
	{
		if ((*iter) == _Obj)
		{
			m_vecParentObj.erase(iter);
			return;
		}
	}

	// 부모 벡터에 없는 게임 오브젝트가 들어왔다면 assert
	assert(nullptr);
}

void CLayer::AddParentList(CGameObject* _Obj)
{
	// 인자로 들어온 게임 오브젝트가 누군가의 자식 오브젝트라면 assert
	assert(!_Obj->m_Parent);

	// 해당 오브젝트를 부모 벡터에 추가한다.
	m_vecParentObj.push_back(_Obj);
}


