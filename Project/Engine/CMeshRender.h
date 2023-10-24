#pragma once
#include "CRenderComponent.h"

// 오브젝트의 Mesh(형태) 를 Material을 통해 렌더링하는 클래스

class CMeshRender :
    public CRenderComponent
{
public:
    CMeshRender();
    ~CMeshRender();

public:
    CLONE(CMeshRender);

public:
    virtual void finaltick() override;
    virtual void render() override;
};

