#pragma once
#include "UI.h"

#include <Engine\ptr.h>
#include <Engine\CResource.h>

class ResourceUI :
    public UI
{
public:
    ResourceUI(RES_TYPE _type);
    ~ResourceUI();

private:
    Ptr<CResource>          m_TargetRes;
    const RES_TYPE          m_ResType;

public:
    virtual int render_update() override;

public:
    void SetTargetRes(Ptr<CResource> _Res) { m_TargetRes = _Res; }
    Ptr<CResource> GetTargetRes() { return m_TargetRes; }
};

