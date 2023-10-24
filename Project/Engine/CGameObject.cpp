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
	// 1. arrCom(컴포넌트 배열) 깊은 복사
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; i++)
	{
		if (_Other.m_arrCom[i])
		{
			AddComponent(_Other.m_arrCom[i]->Clone());
		}
	}

	// 2. Script 깊은 복사
	for (size_t i = 0; i < _Other.m_vecScript.size(); i++)
	{
		AddComponent(_Other.m_vecScript[i]->Clone());
	}

	// 3. 자식 오브젝트 복사
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
	// 레벨이 시작되기 전에 호출될 시점 함수 (플레이 직전 다 준비되어있는 상태에서 멈춰있게끔 만듦)
	// 생성자와는 개념이 다르며, 레벨이 시작된 이후에 생성된 오브젝트들은 생성될 때 begin이 호출되어야함

	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->begin();
	}
	// Script 타입이 CompType End 뒤에 선언되어있어 위 루프문에서 호출되지 않으므로 vecScript만큼 따로 호출한다.
	for (size_t i = 0; i < m_vecScript.size(); ++i)
	{
		m_vecScript[i]->begin();
	}

	// 본인의 연산이 끝나면 자식 벡터를 돌며 재귀호출한다.
	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->begin();
	}
}

void CGameObject::tick()
{
	// 생성된 첫 프레임에만 NewlyCreated값을 true로, 이후에는 false
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

	// Script 타입이 CompType End 뒤에 선언되어있어 위 루프문에서 호출되지 않으므로 vecScript만큼 따로 호출한다.
	for (size_t i = 0; i < m_vecScript.size(); ++i)
	{
		m_vecScript[i]->tick();
	}

	// 본인의 연산이 끝나면 자식 벡터를 돌며 재귀호출한다.
	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->tick();
	}
}

void CGameObject::finaltick()
{
	// 수명 옵션이 켜져있다면, 수명 시간을 초과시 오브젝트 스스로 삭제처리
	if (m_bLifeSpan)
	{
		m_fCurLifeTime += DT;
		if (m_fLifeTime < m_fCurLifeTime)
		{
			DestroyObject(this);
		}
	}

	//Component Type이 Script인 경우 Finaltick 자체를 호출하지 않는다
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::SCRIPT; i++)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->finaltick();
	}

	// 본인의 연산이 끝나면 자식 벡터를 돌며 재귀호출한다.
	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		m_vecChild[i]->finaltick();
	}

	// finaltick 호출이 끝나면, 매 틱마다 자신이 속한 '모든레이어벡터'에 자신(게임 오브젝트)을 등록한다.

	// 소속 레이어가 없는데(-1) finaltick이 호출되는 경우 assert 처리
	assert(-1 != m_iLayerIdx);

	// 레이어 등록
	CLayer* pCurLayer = CLevelManager::GetInst()->GetCurLevel()->GetLayer(m_iLayerIdx);
	pCurLayer->RegisterObject(this);
}

void CGameObject::render()
{
	// 오브젝트에 렌더 기능을 가진 컴포넌트가 없다면 return
	if (nullptr == m_RenderComp)
		return;

	m_RenderComp->render();
}

void CGameObject::AddChild(CGameObject* _Object)
{
	if (_Object->m_Parent)
	{
		// 기존 부모가 있으면 연결 해제 후 연결
		_Object->DisconnectFromParent();
	}

	else
	{
		// 기존 부모가 없으면, 소속 레이어에서 최상위부모 목록에서 제거된 후 연결
		_Object->ChangeToChildType();
	}

	// 부모 자식 연결
	_Object->m_Parent = this;
	m_vecChild.push_back(_Object);
}

void CGameObject::AddComponent(CComponent* _Component)
{
	_Component->m_pOwner = this;

	// 컴포넌트가 스크립트인 경우
	if (COMPONENT_TYPE::SCRIPT == _Component->GetType())
	{
		m_vecScript.push_back((CScript*)_Component);
	}

	else
	{
		// 이미 보유하고 있는 컴포넌트인 경우 Assert (스크립트를 제외한 모든 컴포넌트는 종류당 딱 하나만 가질 수 있다)
		assert(!m_arrCom[(UINT)_Component->GetType()]);

		m_arrCom[(UINT)_Component->GetType()] = _Component;

		// RenderComponent의 경우, 멤버 m_RenderComp 에도 등록
		if (COMPONENT_TYPE::MESHRENDER <= _Component->GetType()		// Comp Type의 enum값이 메시렌더~데칼 사이라면 
			&& _Component->GetType() <= COMPONENT_TYPE::DECAL)		// 해당 컴포넌트는 RenderComponent다
		{
			// 렌더컴포넌트가 이미 있는 경우, assert
			assert(!m_RenderComp);

			// 멤버 자리에 렌더컴포넌트 등록
			m_RenderComp = (CRenderComponent*)_Component;
		}
	}
}

void CGameObject::DisconnectFromParent()
{
	// 최상위 부모 오브젝트에 호출되었다면 리턴 (자식객체에만 사용 가능)
	if (!m_Parent)
		return;

	// 부모의 자식 벡터를 받아와 순회하며 자신을 자식 벡터에서 삭제한다.
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

	// 실제 레이어에 소속된 오브젝트라면
	if (-1 != m_iLayerIdx)
	{
		// 오브젝트가 소속된 레이어가 관리하는 부모 벡터에서 본인을 제거한다.
		CLayer* pLayer = CLevelManager::GetInst()->GetCurLevel()->GetLayer(m_iLayerIdx);
		pLayer->RemoveFromParentList(this);
	}
}

void CGameObject::AddParentList()
{
	// 오브젝트가 소속된 레이어가 관리하는 부모 벡터에서 본인을 추가한다.
	CLayer* pLayer = CLevelManager::GetInst()->GetCurLevel()->GetLayer(m_iLayerIdx);
	pLayer->AddParentList(this);
}

CGameObject* CGameObject::GetChildObjByName(wstring _name)
{
	// 해당 오브젝트의 자식 벡터를 가져와서 이름으로 검색한다.
	const vector<CGameObject*>& vecChild = m_vecChild;
	for (size_t i = 0; i < m_vecChild.size(); ++i)
	{
		// 가장 먼저 찾아진 오브젝트 한 개를 반환한다.
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
	// 본인의 레이어를 -1로 변경
	ChangeLayer(-1);
	
	// 본인을 프리팹으로 등록
	tEvent evn = {};
	
	evn.Type = EVENT_TYPE::ADD_PREFAB;
	evn.wParam = (DWORD_PTR)this;
	
	CEventManager::GetInst()->AddEvent(evn);
}

tGameObjData CGameObject::RecordObjData()
{
	// 현 프레임의 오브젝트 정보 저장
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
	// 오브젝트의 부모의 부모를 타고 올라가며 인자의 오브젝트가 있는지 확인
	CGameObject* pParent = m_Parent;
	while (pParent)
	{
		if (pParent == _Target)
		{
			// 있다면 인자로 들어온 오브젝트가 해당 오브젝트의 조상이라는 뜻
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
