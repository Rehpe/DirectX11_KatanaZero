#pragma once
#include "CComponent.h"

// ���� ����� ���� ������Ʈ�� ���� 

#include "CMesh.h"
#include "CMaterial.h"
#include "ptr.h"

class CRenderComponent :
    public CComponent
{

public:
    CRenderComponent(COMPONENT_TYPE _type);
    virtual ~CRenderComponent();

private:
    Ptr<CMesh>              m_pMesh;
   
    // ������ �پ��ϰ� Ȱ���ϱ� ���� ���� ������ Shared�� Dynamic���� ������.
    //Ptr<CMaterial>          m_pMtrl;

    Ptr<CMaterial>          m_pSharedMtrl;  // ���� ���׸���
    Ptr<CMaterial>          m_pDynamicMtrl; // SharedMaterial ���纻
    Ptr<CMaterial>          m_pCurrentMtrl; // ���� ��� ���� ����

    // ���� ������ ��� ���ҽ��� �ƴ�(���� ������ ���ҽ��̰�, ���ҽ� �Ŵ����� �����Ѵ�)
    // ���� ������ RenderComponent�� �ǽð����� ���� ���̴�, �׷��� Ptr�� �����Ǳ� ������ �˾Ƽ� Release��

public:
    virtual void render() = 0;          // RenderComponent�� ��ӹ��� Ŭ�������� �ݵ�� render �� �����ؾ���

public:
    void SetMesh(Ptr<CMesh> _Mesh) { m_pMesh = _Mesh; }
    void SetMaterial(Ptr<CMaterial> _Mtrl);         // ������ 2�з��� ������ ���� ������

    Ptr<CMesh> GetMesh() { return m_pMesh; }
    Ptr<CMaterial> GetMaterial() { return m_pCurrentMtrl; }
    Ptr<CMaterial> GetSharedMaterial() { return m_pSharedMtrl; }
    Ptr<CMaterial> GetDynamicMaterial();

public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
};

