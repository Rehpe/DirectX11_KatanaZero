#pragma once
#include "ComponentUI.h"

class CTexture;

class ParticleSystemUI :
    public ComponentUI
{
public:
    ParticleSystemUI();
    ~ParticleSystemUI();

private:
    tParticleModule     m_PModule;

    bool                SpawnModule;
    bool                ScaleChangeModule;
    bool                ColorChangeModule;
    bool                AddVelocityModule;
    bool                DragModule;
    bool                NoiseForceModule;
    bool                RenderModule;
    bool                Render_VelocityAlignment;
    bool                Render_VelocityScale;

    Ptr<CTexture>       m_ParticleTex;

public:
    virtual void init() override;
    virtual int render_update() override;

public:
    void CheckParticleTex();
    void CheckSpawnModule();
    void CheckScaleChangeModule();
    void CheckColorChangeModule();
    void CheckAddVelocityModule();
    void CheckDragModule();
    void CheckNoiseForceModule();
    void CheckRenderModule();

public:
    void SelectTexture(DWORD_PTR _Key);

};

