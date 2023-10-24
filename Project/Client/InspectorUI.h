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
    CGameObject*        m_pTargetObj;       // �ش� UI�� ����ϰ� �ִ� ���� ������Ʈ
    ComponentUI*        m_arrComUI[(UINT)COMPONENT_TYPE::END];  // ������ �迭�� �� Component UI ����

    Ptr<CResource>      m_pTargetRes;       // �ش� UI�� ����ϰ� �ִ� ���ҽ�
    ResourceUI*         m_arrResUI[(UINT)RES_TYPE::END];    // ������ �迭�� �� Resource UI ����

    LayerUI*            m_LayerUI;

    vector<ScriptUI*>   m_vecScriptUI;      // ��ũ��Ʈ�� ������ ���� �� �����Ƿ� ���ͷ� ����

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

