#pragma once
#include "UI.h"

// Inspector���� ������, ������Ʈ�� ����ϴ� UI���� ���� �θ�

#include <Engine\ptr.h>
#include <Engine\CResource.h>

class ComponentUI :
    public UI
{
public: 
    // ComponentUI�� ID���Ӹ� �ƴ϶� ������ ����ϴ� Component Ÿ�Ե� �����ڿ� �Է¹޴´�.
    ComponentUI(const string& _Name, COMPONENT_TYPE _Type);
    virtual ~ComponentUI();

private:
    CGameObject*            m_Target;
    const COMPONENT_TYPE    m_Type;         // ������ � ������Ʈ�� ����ϴ���

public:
    virtual void init();
    virtual int render_update() override;

public:
    void SetTarget(CGameObject* _Target);
    CGameObject* GetTarget() { return m_Target; }

    COMPONENT_TYPE GetComponentType() { return m_Type; }


    // ImGui�� Ű������ 1����Ʈ ���ڿ��� �����Ƿ�, �츮 ���ҽ��� 2����Ʈ ���ڿ��� ��ȯ���������
    void GetResKey(Ptr<CResource> _Resource, char* _Buff, size_t _BufferSize);
};

