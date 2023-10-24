#pragma once
#include "UI.h"

// Inspector에서 보여질, 컴포넌트를 담당하는 UI들의 공통 부모

#include <Engine\ptr.h>
#include <Engine\CResource.h>

class ComponentUI :
    public UI
{
public: 
    // ComponentUI는 ID값뿐만 아니라 본인이 담당하는 Component 타입도 생성자에 입력받는다.
    ComponentUI(const string& _Name, COMPONENT_TYPE _Type);
    virtual ~ComponentUI();

private:
    CGameObject*            m_Target;
    const COMPONENT_TYPE    m_Type;         // 본인이 어떤 컴포넌트를 담당하는지

public:
    virtual void init();
    virtual int render_update() override;

public:
    void SetTarget(CGameObject* _Target);
    CGameObject* GetTarget() { return m_Target; }

    COMPONENT_TYPE GetComponentType() { return m_Type; }


    // ImGui는 키값으로 1바이트 문자열을 받으므로, 우리 리소스의 2바이트 문자열을 변환시켜줘야함
    void GetResKey(Ptr<CResource> _Resource, char* _Buff, size_t _BufferSize);
};

