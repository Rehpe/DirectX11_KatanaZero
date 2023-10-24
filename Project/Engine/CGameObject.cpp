#include "pch.h"
#include "CGameObject.h"
#include "CComponent.h"
#include "CMeshRender.h"

#include "CScript.h"

#include "CLevelManager.h"
#include "CLevel.h"
#include "CLayer.h"
#include "CEventManager.h"
#include "CAnim2D.h"

CGameObject::CGameObject()
	: m_arrCom{}
	, m_RenderComp(nullptr)
	, m_Parent(nullptr)
	, m_iLayerIdx(-1)
	, m_bDead(false)
	, m_fLifeTime(0.f)
	, m_fCurLifeTime(0.f)
	, m_bLifeSpan(false)
	, m_iNewlyCreatedCounter(1)
	, m_bNewlyCreated(true)
{
}

CGameObject::CGameObject(const CGameObject& _Other)
	: CEntity(_Other)
	, m_arrCom{}
	, m_RenderComp(nullptr)
	, m_Parent(nullptr)
	, m_iLayerIdx(-1)
	, m_bDead(false)
	, m_fLifeTime(0.f)
	, m_fCurLifeTime(0.f)
	, m_bLifeSpan(false)
	, m_iNewlyCreatedCounter(1)
	, m_bNewlyCreated(true)
{
	// 1. arrCom(������Ʈ �迭) ���� ����
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; i++)
	{
		if (_Other.m_arrCom[i])
		{
			AddComponent(_Other.m_arrCom[i]->Clone());
		}
	}

	// 2. Script ���� ����
	for (size_t i = 0; i < _Other.m_vecScript.size(); i++)
	{
		AddComponent(_Other.m_vecScript[i]->Clone());
	}

	// 3. �ڽ� ������Ʈ ����
	for (size_t i = 0; i < _Other.m_vecChild.size(); i++)
	{
		AddChild(_Other.m_vecChild[i]->Clone());
	}
}

CGameObject::~CGameObject()
{
	Safe_Del_Array(m_arrCom);
	Safe_Del_Vec(m_vecScript);
	//Safe_Del_Vec(m_vecChild);
}

void CGameObject::begin()
{
	// ������ ���۵Ǳ� ���� ȣ��� ���� �Լ� (�÷��� ���� �� �غ�Ǿ��ִ� ���¿��� �����ְԲ� ����)
	// �����ڿʹ� ������ �ٸ���, ������ ���۵� ���Ŀ� ������ ������Ʈ���� ������ �� begin�� ȣ��Ǿ����

	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->begin();
	}
	// Script Ÿ���� CompType End �ڿ� ����Ǿ��־� �� ���������� ȣ����� �����Ƿ� vecScript��ŭ ���� ȣ���Ѵ�.
	for (size_t i = 0; i < m_vecScript.size(); ++i)
	{
		m_vecScript[i]->begin();
	}

	// ������ ������ ������ �ڽ� ���͸� ���� ���ȣ���Ѵ�.
	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->begin();
	}
}

void CGameObject::tick()
{
	// ������ ù �����ӿ��� NewlyCreated���� true��, ���Ŀ��� false
	if (m_iNewlyCreatedCounter >= 0 && m_bNewlyCreated)
	{
		--m_iNewlyCreatedCounter;
		if (m_iNewlyCreatedCounter < 0)
		{
			m_bNewlyCreated = false;
		}
	}

	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; i++)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->tick();
	}

	// Script Ÿ���� CompType End �ڿ� ����Ǿ��־� �� ���������� ȣ����� �����Ƿ� vecScript��ŭ ���� ȣ���Ѵ�.
	for (size_t i = 0; i < m_vecScript.size(); ++i)
	{
		m_vecScript[i]->tick();
	}

	// ������ ������ ������ �ڽ� ���͸� ���� ���ȣ���Ѵ�.
	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->tick();
	}
}

void CGameObject::finaltick()
{
	// ���� �ɼ��� �����ִٸ�, ���� �ð��� �ʰ��� ������Ʈ ������ ����ó��
	if (m_bLifeSpan)
	{
		m_fCurLifeTime += DT;
		if (m_fLifeTime < m_fCurLifeTime)
		{
			DestroyObject(this);
		}
	}

	//Component Type�� Script�� ��� Finaltick ��ü�� ȣ������ �ʴ´�
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::SCRIPT; i++)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->finaltick();
	}

	// ������ ������ ������ �ڽ� ���͸� ���� ���ȣ���Ѵ�.
	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->finaltick();
	}

	// finaltick ȣ���� ������, �� ƽ���� �ڽ��� ���� '��緹�̾��'�� �ڽ�(���� ������Ʈ)�� ����Ѵ�.

	// �Ҽ� ���̾ ���µ�(-1) finaltick�� ȣ��Ǵ� ��� assert ó��
	assert(-1 != m_iLayerIdx);

	// ���̾� ���
	CLayer* pCurLayer = CLevelManager::GetInst()->GetCurLevel()->GetLayer(m_iLayerIdx);
	pCurLayer->RegisterObject(this);
}

void CGameObject::render()
{
	// ������Ʈ�� ���� ����� ���� ������Ʈ�� ���ٸ� return
	if (nullptr == m_RenderComp)
		return;

	m_RenderComp->render();
}

void CGameObject::AddChild(CGameObject* _Object)
{
	if (_Object->m_Parent)
	{
		// ���� �θ� ������ ���� ���� �� ����
		_Object->DisconnectFromParent();
	}

	else
	{
		// ���� �θ� ������, �Ҽ� ���̾�� �ֻ����θ� ��Ͽ��� ���ŵ� �� ����
		_Object->ChangeToChildType();
	}

	// �θ� �ڽ� ����
	_Object->m_Parent = this;
	m_vecChild.push_back(_Object);
}

void CGameObject::AddComponent(CComponent* _Component)
{
	_Component->m_pOwner = this;

	// ������Ʈ�� ��ũ��Ʈ�� ���
	if (COMPONENT_TYPE::SCRIPT == _Component->GetType())
	{
		m_vecScript.push_back((CScript*)_Component);
	}

	else
	{
		// �̹� �����ϰ� �ִ� ������Ʈ�� ��� Assert (��ũ��Ʈ�� ������ ��� ������Ʈ�� ������ �� �ϳ��� ���� �� �ִ�)
		assert(!m_arrCom[(UINT)_Component->GetType()]);

		m_arrCom[(UINT)_Component->GetType()] = _Component;

		// RenderComponent�� ���, ��� m_RenderComp ���� ���
		if (COMPONENT_TYPE::MESHRENDER <= _Component->GetType()		// Comp Type�� enum���� �޽÷���~��Į ���̶�� 
			&& _Component->GetType() <= COMPONENT_TYPE::DECAL)		// �ش� ������Ʈ�� RenderComponent��
		{
			// ����������Ʈ�� �̹� �ִ� ���, assert
			assert(!m_RenderComp);

			// ��� �ڸ��� ����������Ʈ ���
			m_RenderComp = (CRenderComponent*)_Component;
		}
	}
}

void CGameObject::DisconnectFromParent()
{
	// �ֻ��� �θ� ������Ʈ�� ȣ��Ǿ��ٸ� ���� (�ڽİ�ü���� ��� ����)
	if (!m_Parent)
		return;

	// �θ��� �ڽ� ���͸� �޾ƿ� ��ȸ�ϸ� �ڽ��� �ڽ� ���Ϳ��� �����Ѵ�.
	vector<CGameObject*>::iterator iter = m_Parent->m_vecChild.begin();
	for (; iter != m_Parent->m_vecChild.end(); ++iter)
	{
		if (this == *iter)
		{
			m_Parent->m_vecChild.erase(iter);
			m_Parent = nullptr;
			return;
		}
	}

	assert(nullptr);
}

void CGameObject::ChangeToChildType()
{
	assert(-1 <= m_iLayerIdx && m_iLayerIdx < MAX_LAYER);

	// ���� ���̾ �Ҽӵ� ������Ʈ���
	if (-1 != m_iLayerIdx)
	{
		// ������Ʈ�� �Ҽӵ� ���̾ �����ϴ� �θ� ���Ϳ��� ������ �����Ѵ�.
		CLayer* pLayer = CLevelManager::GetInst()->GetCurLevel()->GetLayer(m_iLayerIdx);
		pLayer->RemoveFromParentList(this);
	}
}

void CGameObject::AddParentList()
{
	// ������Ʈ�� �Ҽӵ� ���̾ �����ϴ� �θ� ���Ϳ��� ������ �߰��Ѵ�.
	CLayer* pLayer = CLevelManager::GetInst()->GetCurLevel()->GetLayer(m_iLayerIdx);
	pLayer->AddParentList(this);
}

CGameObject* CGameObject::GetChildObjByName(wstring _name)
{
	// �ش� ������Ʈ�� �ڽ� ���͸� �����ͼ� �̸����� �˻��Ѵ�.
	const vector<CGameObject*>& vecChild = m_vecChild;
	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		// ���� ���� ã���� ������Ʈ �� ���� ��ȯ�Ѵ�.
		if (_name == m_vecChild[i]->GetName())
			return m_vecChild[i];
	}

	return nullptr;
}

void CGameObject::ChangeLayer(int _NewLayerIdx)
{
	tEvent evn = {};

	evn.Type = EVENT_TYPE::CHANGE_LAYER;
	evn.wParam = (DWORD_PTR)this;
	evn.lParam = (DWORD_PTR)_NewLayerIdx;

	CEventManager::GetInst()->AddEvent(evn);
}

void CGameObject::AddPrefab()
{
	// ������ ���̾ -1�� ����
	ChangeLayer(-1);
	
	// ������ ���������� ���
	tEvent evn = {};
	
	evn.Type = EVENT_TYPE::ADD_PREFAB;
	evn.wParam = (DWORD_PTR)this;
	
	CEventManager::GetInst()->AddEvent(evn);
}

tGameObjData CGameObject::RecordObjData()
{
	// �� �������� ������Ʈ ���� ����
	m_CurFrmGameObjData = {};


	m_CurFrmGameObjData.ID = GetID();
	m_CurFrmGameObjData.ObjName = GetName();
	m_CurFrmGameObjData.Position = Transform()->GetRelativePos();
	m_CurFrmGameObjData.Scale = Transform()->GetRelativeScale();
	m_CurFrmGameObjData.Rotation = Transform()->GetRelativeRot();
	if (Animator2D() && Animator2D()->GetCurAnim())
	{
		m_CurFrmGameObjData.AnimUse = true;
		m_CurFrmGameObjData.CurAnimName = Animator2D()->GetCurAnim()->GetName();
		m_CurFrmGameObjData.CurAnimFrm = Animator2D()->GetCurAnim()->GetCurFrameint();
	}
	else
		m_CurFrmGameObjData.AnimUse = false;
	if (Light2D())
	{
		m_CurFrmGameObjData.LightUse = true;
		m_CurFrmGameObjData.LightDiffuse = Light2D()->GetLightDiffuse();
	}
	if (MeshRender())
	{
		m_CurFrmGameObjData.MeshRenderUse = true;
		if (MeshRender()->GetMaterial() != nullptr)
			m_CurFrmGameObjData.MtrlName = MeshRender()->GetMaterial()->GetKey();
		else
			m_CurFrmGameObjData.MtrlName = L"";
	}
	else
		m_CurFrmGameObjData.MeshRenderUse = false;
		
	m_CurFrmGameObjData.IsDead = m_bDead;
	if(m_bNewlyCreated)
		m_CurFrmGameObjData.IsNewlyCreated = m_bNewlyCreated;

	return m_CurFrmGameObjData;
}

void CGameObject::ApplyObjData(tGameObjData _Data)
{
	Transform()->SetRelativePos(_Data.Position);
	Transform()->SetRelativeScale(_Data.Scale);
	Transform()->SetRelativeRot(_Data.Rotation);
	if(_Data.AnimUse)
	{
		Animator2D()->Play(_Data.CurAnimName, false);
		Animator2D()->GetCurAnim()->SetCurFrame(_Data.CurAnimFrm);
	}
	if (_Data.LightUse)
	{
		Light2D()->SetLightDiffuse(_Data.LightDiffuse);
	}
	if (_Data.MeshRenderUse)
	{
		Ptr<CMaterial> pMtrl;
		if(_Data.MtrlName == L"")
			pMtrl = nullptr;
		else
			pMtrl = CResourceManager::GetInst()->FindRes<CMaterial>(_Data.MtrlName);
		
		MeshRender()->SetMaterial(pMtrl);
	}
}

bool CGameObject::IsAncestor(CGameObject* _Target)
{
	// ������Ʈ�� �θ��� �θ� Ÿ�� �ö󰡸� ������ ������Ʈ�� �ִ��� Ȯ��
	CGameObject* pParent = m_Parent;
	while (pParent)
	{
		if (pParent == _Target)
		{
			// �ִٸ� ���ڷ� ���� ������Ʈ�� �ش� ������Ʈ�� �����̶�� ��
			return true;
		}
		pParent = pParent->m_Parent;
	}
	return false;
}

wstring CGameObject::GetLayerName()
{
	CLayer* pLayer = CLevelManager::GetInst()->GetCurLevel()->GetLayer(m_iLayerIdx);
	return pLayer->GetName();
}
