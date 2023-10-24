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
    CStructuredBuffer*          m_ParticleBuffer;   // 각각 파티클 하나의 정보가 들어있는 버퍼
    CStructuredBuffer*          m_RWBuffer;         // 모든 파티클들이 공유하는 전체적인 변수(스폰시킬 파티클 개수)
   
    tParticleModule             m_ModuleData;       // 파티클 모듈에 필요한 데이터 변수들
    CStructuredBuffer*          m_ModuleDataBuffer; // 위 구조체를 바인딩할 구조화 버퍼

    Ptr<CParticleUpdateShader>  m_UpdateCS;

    Ptr<CTexture>               m_ParticleTex;
    float                       m_AccTime;          // 누적시간

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

