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
    // �� ������Ʈ���� Save/Load �ʿ��� �׸� �������̵��Ͽ� �ۼ�(���������Լ�)
    virtual void SaveToLevelFile(FILE* _File) = 0;
    virtual void LoadFromLevelFile(FILE* _FILE) = 0;

public:
    // ������Ʈ�� �ٸ� ������Ʈ�� ���� ���ϰ� �˾ƿ� �� �ֵ��� Get �Լ� �ۼ�
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

