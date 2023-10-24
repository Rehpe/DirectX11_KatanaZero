#pragma once
#include "CComponent.h"

// 자주 사용하는 Key, Time, Resource, Level 매니저는 아예 헤더에서 참조한다.

#include "CTimeManager.h"
#include "CKeyManager.h"
#include "CResourceManager.h"
#include "CLevelManager.h"

// 스크립트가 구현시킬 본인(오브젝트)과 각종 component를 참조한다.

#include "CGameObject.h"
#include "component.h"

enum class SCRIPT_PARAM     // 스크립트가 표시할 파라미터의 타입
{
    INT,
    FLOAT,
    VEC2,
    VEC4,
};

struct tScriptParam         
{
    SCRIPT_PARAM    eParam;     // 파라미터 타입
    void*           pData;      // 실제 데이터
    string          strDesc;    // 파라미터 설명
};

enum class CollisionDir     // 충돌 방향
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
};

class CScript :
    public CComponent
{

public:
    CScript(UINT _ScriptType);
    virtual ~CScript();

public:
    CLONE(CScript);

private:
    // 본인의 스크립트 타입을 UINT 타입으로 들고 있는다
    // enum값은 script프로젝트에 구현되어있으므로 기본타입 UINT로 선언
    UINT                    m_iScriptType;     

    // 에디터에서 보여줄 파라미터를 관리하는 벡터
    vector<tScriptParam>    m_vecParam;

public:
    virtual void finaltick() final {};

    // Script의 경우 finaltick 구현 금지
    // tick에서는 게임 로직 구현, finaltick에서는 tick의 결과값을 마무리하며 다이렉트/GPU 메모리 관련하여 셋팅(상수버퍼, 바인딩 등..)
    // Script에서는 오로지 오브젝트의 로직만을 구현할 것이므로 final 키워드를 사용하여 finaltick 구현을 금지시킬 것이다
    // 또한 GameObject의 finaltick에서도 컴포넌트 타입이 Script이면 finaltick 자체를 호출하지 않는 식으로 이중 금지를 시킬 것이다

public:
    virtual void BeginOverlap(CCollider2D* _Other) {}
    virtual void OnOverlap(CCollider2D* _Other) {}
    virtual void EndOverlap(CCollider2D* _Other) {}

protected:
    void AddScriptParam(SCRIPT_PARAM eParam, void* _pData, const string& _Desc);

public:
    void Destroy() { DestroyObject(GetOwner()); }   // 해당 스크립트가 붙은 오브젝트를 스스로 삭제하는 함수
    void SetLifeSpan(float _Time) { GetOwner()->SetLifeSpan(_Time); }   // 해당 스크립트가 붙은 오브젝트에 수명 부여
    UINT GetScriptType() { return m_iScriptType; }
    const vector<tScriptParam>& GetScritpParam() { return m_vecParam; }

public:
    virtual void SaveToLevelFile(FILE* _File) override {}
    virtual void LoadFromLevelFile(FILE* _File) override {}

public:
    Vec3 CalculateOverlapAmount(CCollider2D* _colliderA, CCollider2D* _colliderB);
    CollisionDir GetCollisionDirX(CCollider2D* _colliderA, CCollider2D* _colliderB);  // A 기준, B의 좌/우 어디에서 부딪혔는지
    CollisionDir GetCollisionDirY(CCollider2D* _colliderA, CCollider2D* _colliderB);  // A 기준, B의 상/하 어디에서 부딪혔는지
};

