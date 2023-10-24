#pragma once
#include "CRenderComponent.h"

#include "ptr.h"
#include "CParticleUpdateShader.h"

class CTexture;
class CStructuredBuffer;

class CParticleSystem :
    public CRenderComponent
{
public:
    CParticleSystem();
    ~CParticleSystem();

public:
    CLONE(CParticleSystem);

private:
    CStructuredBuffer*          m_ParticleBuffer;   // ���� ��ƼŬ �ϳ��� ������ ����ִ� ����
    CStructuredBuffer*          m_RWBuffer;         // ��� ��ƼŬ���� �����ϴ� ��ü���� ����(������ų ��ƼŬ ����)
   
    tParticleModule             m_ModuleData;       // ��ƼŬ ��⿡ �ʿ��� ������ ������
    CStructuredBuffer*          m_ModuleDataBuffer; // �� ����ü�� ���ε��� ����ȭ ����

    Ptr<CParticleUpdateShader>  m_UpdateCS;

    Ptr<CTexture>               m_ParticleTex;
    float                       m_AccTime;          // �����ð�

public:
    virtual void finaltick() override;
    virtual void render() override;

public:
    const tParticleModule& GetModule() { return m_ModuleData; };
    void SetModule(tParticleModule Module) { m_ModuleData = Module; };

    void SetParticleTex(Ptr<CTexture> _Tex) { m_ParticleTex = _Tex; }
    Ptr<CTexture> GetParticleTex() { return m_ParticleTex; }

public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

};

