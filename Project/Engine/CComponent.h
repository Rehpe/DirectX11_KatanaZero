#pragma once
#include "CEntity.h"

#include "CGameObject.h"

class CTransform;
class CMeshRender;

#define GET_OTHER_COMPONENT(Type) C##Type* Type() {return m_pOwner->Type();}

class CComponent :
    public CEntity
{
public:
    CComponent(COMPONENT_TYPE _Type);
    CComponent(const CComponent& _Other);
    virtual ~CComponent();

public:
    virtual CComponent* Clone() = 0;

private:
    const COMPONENT_TYPE        m_Type;
    CGameObject*                m_pOwner;

public:
    virtual void begin() {};
    virtual void tick() {};
    virtual void finaltick() = 0;

public:
    COMPONENT_TYPE GetType() { return m_Type; }
    CGameObject* GetOwner() { return m_pOwner; };

public:
    // 각 컴포넌트마다 Save/Load 필요한 항목 오버라이드하여 작성(순수가상함수)
    virtual void SaveToLevelFile(FILE* _File) = 0;
    virtual void LoadFromLevelFile(FILE* _FILE) = 0;

public:
    // 컴포넌트가 다른 컴포넌트를 보다 편하게 알아올 수 있도록 Get 함수 작성
    GET_OTHER_COMPONENT(Transform);
    GET_OTHER_COMPONENT(MeshRender);
    GET_OTHER_COMPONENT(Camera);
    GET_OTHER_COMPONENT(Collider2D);
    GET_OTHER_COMPONENT(Light2D);
    GET_OTHER_COMPONENT(TileMap);
    GET_OTHER_COMPONENT(Animator2D);
    GET_OTHER_COMPONENT(ParticleSystem);
    GET_OTHER_COMPONENT(RigidBody2D);
    GET_OTHER_COMPONENT(FSM);



    friend class CGameObject;
};

