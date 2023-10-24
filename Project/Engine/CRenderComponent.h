#pragma once
#include "CComponent.h"

// 렌더 기능을 가진 컴포넌트의 공통 

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
   
    // 재질을 다양하게 활용하기 위해 기존 재질을 Shared와 Dynamic으로 나눈다.
    //Ptr<CMaterial>          m_pMtrl;

    Ptr<CMaterial>          m_pSharedMtrl;  // 원본 메테리얼
    Ptr<CMaterial>          m_pDynamicMtrl; // SharedMaterial 복사본
    Ptr<CMaterial>          m_pCurrentMtrl; // 현재 사용 중인 재질

    // 동적 재질의 경우 리소스가 아님(원본 재질만 리소스이고, 리소스 매니저가 관리한다)
    // 동적 재질은 RenderComponent가 실시간으로 만들어낸 것이다, 그러나 Ptr로 관리되기 때문에 알아서 Release됨

public:
    virtual void render() = 0;          // RenderComponent를 상속받은 클래스들은 반드시 render 을 구현해야함

public:
    void SetMesh(Ptr<CMesh> _Mesh) { m_pMesh = _Mesh; }
    void SetMaterial(Ptr<CMaterial> _Mtrl);         // 재질이 2분류로 나뉨에 따라 재정의

    Ptr<CMesh> GetMesh() { return m_pMesh; }
    Ptr<CMaterial> GetMaterial() { return m_pCurrentMtrl; }
    Ptr<CMaterial> GetSharedMaterial() { return m_pSharedMtrl; }
    Ptr<CMaterial> GetDynamicMaterial();

public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;
};

