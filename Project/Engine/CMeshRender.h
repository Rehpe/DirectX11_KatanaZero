#pragma once
#include "CRenderComponent.h"

// ������Ʈ�� Mesh(����) �� Material�� ���� �������ϴ� Ŭ����

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

