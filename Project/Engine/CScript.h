#pragma once
#include "CComponent.h"

// ���� ����ϴ� Key, Time, Resource, Level �Ŵ����� �ƿ� ������� �����Ѵ�.

#include "CTimeManager.h"
#include "CKeyManager.h"
#include "CResourceManager.h"
#include "CLevelManager.h"

// ��ũ��Ʈ�� ������ų ����(������Ʈ)�� ���� component�� �����Ѵ�.

#include "CGameObject.h"
#include "component.h"

enum class SCRIPT_PARAM     // ��ũ��Ʈ�� ǥ���� �Ķ������ Ÿ��
{
    INT,
    FLOAT,
    VEC2,
    VEC4,
};

struct tScriptParam         
{
    SCRIPT_PARAM    eParam;     // �Ķ���� Ÿ��
    void*           pData;      // ���� ������
    string          strDesc;    // �Ķ���� ����
};

enum class CollisionDir     // �浹 ����
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
    // ������ ��ũ��Ʈ Ÿ���� UINT Ÿ������ ��� �ִ´�
    // enum���� script������Ʈ�� �����Ǿ������Ƿ� �⺻Ÿ�� UINT�� ����
    UINT                    m_iScriptType;     

    // �����Ϳ��� ������ �Ķ���͸� �����ϴ� ����
    vector<tScriptParam>    m_vecParam;

public:
    virtual void finaltick() final {};

    // Script�� ��� finaltick ���� ����
    // tick������ ���� ���� ����, finaltick������ tick�� ������� �������ϸ� ���̷�Ʈ/GPU �޸� �����Ͽ� ����(�������, ���ε� ��..)
    // Script������ ������ ������Ʈ�� �������� ������ ���̹Ƿ� final Ű���带 ����Ͽ� finaltick ������ ������ų ���̴�
    // ���� GameObject�� finaltick������ ������Ʈ Ÿ���� Script�̸� finaltick ��ü�� ȣ������ �ʴ� ������ ���� ������ ��ų ���̴�

public:
    virtual void BeginOverlap(CCollider2D* _Other) {}
    virtual void OnOverlap(CCollider2D* _Other) {}
    virtual void EndOverlap(CCollider2D* _Other) {}

protected:
    void AddScriptParam(SCRIPT_PARAM eParam, void* _pData, const string& _Desc);

public:
    void Destroy() { DestroyObject(GetOwner()); }   // �ش� ��ũ��Ʈ�� ���� ������Ʈ�� ������ �����ϴ� �Լ�
    void SetLifeSpan(float _Time) { GetOwner()->SetLifeSpan(_Time); }   // �ش� ��ũ��Ʈ�� ���� ������Ʈ�� ���� �ο�
    UINT GetScriptType() { return m_iScriptType; }
    const vector<tScriptParam>& GetScritpParam() { return m_vecParam; }

public:
    virtual void SaveToLevelFile(FILE* _File) override {}
    virtual void LoadFromLevelFile(FILE* _File) override {}

public:
    Vec3 CalculateOverlapAmount(CCollider2D* _colliderA, CCollider2D* _colliderB);
    CollisionDir GetCollisionDirX(CCollider2D* _colliderA, CCollider2D* _colliderB);  // A ����, B�� ��/�� ��𿡼� �ε�������
    CollisionDir GetCollisionDirY(CCollider2D* _colliderA, CCollider2D* _colliderB);  // A ����, B�� ��/�� ��𿡼� �ε�������
};

