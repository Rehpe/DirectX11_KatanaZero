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

// begin, tick, finaltick�� �ϰ������� �ֻ��� �θ� ���� ������Ʈ���� ȣ���Ѵ�.
// CGameObject ������ ���������� �ڽĺ��͸� ���� ���ȣ���ϹǷ� �θ�->�ڽ� ������ �ùٸ��� ȣ��ȴ�

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
	// Dead ������ �θ� ������Ʈ�� ���Ϳ��� finaltick ȣ�� �� �����Ѵ�.
	// �θ� ���ŵȴٴ� ���� �� ���� ��� �ڽĵ鵵 �Բ� ���ŵȴٴ� ��.

	vector<CGameObject*>::iterator iter = m_vecParentObj.begin();
	for (; iter != m_vecParentObj.end(); )
	{
		(*iter)->finaltick();
		
		// ��� ������Ʈ�� ����� finaltick���� �Ͼ��.
		// Dead ������ �ֻ��� �θ� ������Ʈ�� �� �������� finaltick�� ��ġ�� �����Ǿ��
		// �浹 ���� �������� ������ ������ �ʴ´�.
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
	// �θ� ���Ϳ� �����Ѵ�.
	m_vecParentObj.push_back(_Object);

	// �ش� ���� ������Ʈ���� �ڽ� ������Ʈ�� �ִ��� queue�� ���� ��ȸ�ϸ� �˾Ƴ���.
	static list<CGameObject*> queue;
	queue.clear();

	queue.push_back(_Object);

	while (!queue.empty())	// ť�� �� ������
	{
		CGameObject* pObject = queue.front();	// �� ���� ������Ʈ�� ������
		queue.pop_front();						// �ش� ������Ʈ ť���� ����

		for (size_t i = 0; i < pObject->m_vecChild.size(); i++)
		{
			queue.push_back(pObject->m_vecChild[i]);		// ��� �ڽ��� ť�� ����
		}

		// ������ ������Ʈ�� �ֻ��� �θ�ų�, ���̾� ������ �����̰ų�, �θ�� ���� �̵� �ɼ�(Move)�� true�� ���
		// �θ� ����ϰ��� �ߴ� ���̾�� �Ȱ��� ���̾� ��ȣ�� �����Ѵ�
		if (nullptr == pObject->m_Parent ||				// ������Ʈ�� �ֻ��� �θ�
				 -1 == pObject->m_iLayerIdx ||			// ������Ʈ�� ���̾� ������ ����(����Ʈ�� -1)
				_bMove)									// �θ��� ���̾� ���� �̵� �ɼ� true
			pObject->m_iLayerIdx = m_iLayerIdx;
	}
}

void CLayer::RemoveFromParentList(CGameObject* _Obj)
{
	// �θ� ���Ϳ��� ���ڷ� ���� ���� ������Ʈ�� �����Ѵ�.
	vector<CGameObject*>::iterator iter = m_vecParentObj.begin();
	for (; iter != m_vecParentObj.end(); ++iter)
	{
		if ((*iter) == _Obj)
		{
			m_vecParentObj.erase(iter);
			return;
		}
	}

	// �θ� ���Ϳ� ���� ���� ������Ʈ�� ���Դٸ� assert
	assert(nullptr);
}

void CLayer::AddParentList(CGameObject* _Obj)
{
	// ���ڷ� ���� ���� ������Ʈ�� �������� �ڽ� ������Ʈ��� assert
	assert(!_Obj->m_Parent);

	// �ش� ������Ʈ�� �θ� ���Ϳ� �߰��Ѵ�.
	m_vecParentObj.push_back(_Obj);
}


