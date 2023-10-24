#pragma once
#include "CEntity.h"

class CComponent;
class CTransform;
class CMeshRender;
class CCamera;
class CRenderComponent;
class CScript;
class CCollider2D;
class CLight2D;
class CTileMap;
class CAnimator2D;
class CParticleSystem;
class CRigidBody2D;
class CFSM;

#define GET_COMPONENT(Type, TYPE) C##Type* Type() const { return (C##Type*)m_arrCom[(UINT)COMPONENT_TYPE::TYPE]; }

class CGameObject :
    public CEntity
{
public:
    CGameObject(); 
    CGameObject(const CGameObject& _Other);
    ~CGameObject();

    friend class CLayer;
    friend class CEventManager;

public:
    CLONE(CGameObject);

private:
    CComponent*             m_arrCom[(UINT)COMPONENT_TYPE::END];
    CRenderComponent*       m_RenderComp;       // 게임 오브젝트는 렌더 기능을 가진 컴포넌트를 한 개까지만 가질 수 있다 
    vector<CScript*>        m_vecScript;        // 스크립트를 여러개 가질 수 있도록 스크립트 벡터를 따로 선언한다 (Script Multi)

    // Object Hierachy (오브젝트 계층구조)
    CGameObject*            m_Parent;
    vector<CGameObject*>    m_vecChild;

    int                     m_iLayerIdx;    // 자신이 속한 레이어 번호

    int                     m_iNewlyCreatedCounter;
    bool                    m_bNewlyCreated;    // 방금 생성되었는가
    bool                    m_bDead;
    float                   m_fLifeTime;
    float                   m_fCurLifeTime;
    bool                    m_bLifeSpan;

    tGameObjData            m_CurFrmGameObjData;

public:
    void begin();               // 레벨이 시작될 때 호출, 또는 오브젝트가 이미 시작된 레벨에 합류할 때 호출
    void tick();
    virtual void finaltick();
    void render();
        

public:    
    // Object Hierachy 오브젝트 계층 구조
    void AddChild(CGameObject* _Object);

    const vector<CGameObject*> GetChild() { return m_vecChild; }
    CGameObject* GetChildObjByName(wstring _name);
    CGameObject* GetParent() const { return m_Parent; }
    void ChangeLayer(int _NewLayerIdx);
    void AddPrefab();

    tGameObjData RecordObjData();
    void ApplyObjData(tGameObjData _Data);

private:
    void DisconnectFromParent();
    void ChangeToChildType();
    void AddParentList();
    

public:
    void AddComponent(CComponent* _Component);
    
    GET_COMPONENT(Transform, TRANSFORM);
    GET_COMPONENT(MeshRender, MESHRENDER);
    GET_COMPONENT(Camera, CAMERA);
    GET_COMPONENT(Collider2D, COLLIDER2D);
    GET_COMPONENT(Light2D, LIGHT2D);
    GET_COMPONENT(TileMap, TILEMAP);
    GET_COMPONENT(Animator2D, ANIMATOR2D);
    GET_COMPONENT(ParticleSystem, PARTICLESYSTEM);
    GET_COMPONENT(RigidBody2D, RIGIDBODY2D);
    GET_COMPONENT(FSM, FSM_AI);

    CComponent* GetComponent(COMPONENT_TYPE _CompType) { return m_arrCom[(UINT)_CompType]; }
    void DeleteComponent(COMPONENT_TYPE _CompType) { m_arrCom[(UINT)_CompType] = nullptr; }

    CRenderComponent* GetRenderComponent() const { return m_RenderComp; }

    int GetLayerIdx() { return m_iLayerIdx; }
    wstring GetLayerName();

    // 게임 오브젝트에 달린 모든 스크립트 가져오는 기능
    const vector<CScript*>& GetScripts() { return m_vecScript; }

    // 게임 오브젝트에 달린 특정 스크립트 하나를 가져오는 기능
    template<typename T>
    T* GetScript();

    void SetLifeSpan(float _fTime)
    {
        if (m_bLifeSpan)
            return;

        m_fLifeTime = _fTime;
        m_bLifeSpan = true;
    }

    bool IsNewlyCreated() { return m_bNewlyCreated; }
    bool IsDead() { return m_bDead; }
    bool IsAncestor(CGameObject* _Target);

};

template<typename T>
inline T* CGameObject::GetScript()
{
    for (size_t i = 0; i < m_vecScript.size(); ++i)
    {
        T* pScript = dynamic_cast<T*> (m_vecScript[i]);
        if (nullptr != pScript)
            return pScript;
    }

    return nullptr;
}



