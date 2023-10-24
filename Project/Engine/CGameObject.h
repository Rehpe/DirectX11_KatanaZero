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
    CRenderComponent*       m_RenderComp;       // ���� ������Ʈ�� ���� ����� ���� ������Ʈ�� �� �������� ���� �� �ִ� 
    vector<CScript*>        m_vecScript;        // ��ũ��Ʈ�� ������ ���� �� �ֵ��� ��ũ��Ʈ ���͸� ���� �����Ѵ� (Script Multi)

    // Object Hierachy (������Ʈ ��������)
    CGameObject*            m_Parent;
    vector<CGameObject*>    m_vecChild;

    int                     m_iLayerIdx;    // �ڽ��� ���� ���̾� ��ȣ

    int                     m_iNewlyCreatedCounter;
    bool                    m_bNewlyCreated;    // ��� �����Ǿ��°�
    bool                    m_bDead;
    float                   m_fLifeTime;
    float                   m_fCurLifeTime;
    bool                    m_bLifeSpan;

    tGameObjData            m_CurFrmGameObjData;

public:
    void begin();               // ������ ���۵� �� ȣ��, �Ǵ� ������Ʈ�� �̹� ���۵� ������ �շ��� �� ȣ��
    void tick();
    virtual void finaltick();
    void render();
        

public:    
    // Object Hierachy ������Ʈ ���� ����
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

    // ���� ������Ʈ�� �޸� ��� ��ũ��Ʈ �������� ���
    const vector<CScript*>& GetScripts() { return m_vecScript; }

    // ���� ������Ʈ�� �޸� Ư�� ��ũ��Ʈ �ϳ��� �������� ���
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



