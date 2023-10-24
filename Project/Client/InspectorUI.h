#pragma once
#include "UI.h"

class CGameObject;
class ComponentUI;
class ResourceUI;
class ScriptUI;
class LayerUI;

#include <Engine\ptr.h>
#include <Engine\CResource.h>

class InspectorUI :
    public UI
{
public:
    InspectorUI();
    ~InspectorUI();

private:
    CGameObject*        m_pTargetObj;       // 해당 UI가 담당하고 있는 게임 오브젝트
    ComponentUI*        m_arrComUI[(UINT)COMPONENT_TYPE::END];  // 포인터 배열로 각 Component UI 관리

    Ptr<CResource>      m_pTargetRes;       // 해당 UI가 담당하고 있는 리소스
    ResourceUI*         m_arrResUI[(UINT)RES_TYPE::END];    // 포인터 배열로 각 Resource UI 관리

    LayerUI*            m_LayerUI;

    vector<ScriptUI*>   m_vecScriptUI;      // 스크립트는 여러개 가질 수 있으므로 벡터로 관리

public:
    virtual void init() override;
    virtual void tick() override;
    virtual int render_update() override;

public:
    void SetTargetObject(CGameObject* _Target);
    void SetTargetResource(Ptr<CResource> _Res);


private:
    void ClearTargetObject();
    void ClearTargetResource();


};

