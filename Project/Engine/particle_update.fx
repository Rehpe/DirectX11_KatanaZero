#ifndef _PARTICLE_UPDATE
#define _PARTICLE_UPDATE

#include "value.fx"
#include "struct.fx"
#include "func.fx"

RWStructuredBuffer<tParticle> ParticleBuffer : register(u0);
RWStructuredBuffer<int4> ParticleSpawnCount : register(u1);
StructuredBuffer<tParticleModule> ParticleModuleData : register(t20);
Texture2D NoiseTexture : register(t21);


#define ObjectPos           g_vec4_0

#define NoiseTexResolution  g_vec2_0

#define SpawnCount          ParticleSpawnCount[0].x
#define ModuleData          ParticleModuleData[0]
#define ParticleMaxCount    ParticleModuleData[0].iMaxParticleCount

#define SpawnModule         ParticleModuleData[0].Spawn
#define ColorChangeModule   ParticleModuleData[0].ColorChange
#define ScaleChangeModule   ParticleModuleData[0].ScaleChange


// numsthread�� 1���� �����ϴ� ����?
// DispatchThreadID.x�� ����Ͽ� �� ID������ ��ƼŬ�� �����ϱ� ����
[numthreads(128, 1, 1)]
void CS_ParticleUpdate(int3 _ID : SV_DispatchThreadID)
{
    // ��ƼŬ �ִ� ������ �Ѿ�� �� �̻� �������� �ʰ� return
    if (ParticleMaxCount <= _ID.x)
        return;
       
    tParticle particle = ParticleBuffer[_ID.x];
    
    if (SpawnModule) // ���� ��� Ȱ��ȭ��
    {
     // ��ƼŬ�� ��Ȱ��ȭ ������ ���
        if (particle.Active == 0)
        {
        // SpawnCount �� 0 �̻��̶��, ��ƼŬ�� Ȱ��ȭ��Ŵ      
            while (0 < SpawnCount)
            {
                int orgvalue = SpawnCount;
                int outvalue = 0;
            
            // ����1: ���� ����� ������ (��ƼŬ�� �ϳ� �����ϰ� SpawnCount���� -1�� ������ϹǷ�)
            // ����2: �������� ���� ���� ��. ����1 = ����2 �� ��쿡�� ����1�� ����3������ ��ü�Ѵ�.
            // ����3: Value(�������� ���� ��ȯ�� ��) : SpawnCount - 1
            // ����4: Out(���۷���, ����� ����ִ� ����) : original, �������� ���� ����ִ� int�� 
            // (���� SpawnCount�� ����ִ� ���� �ش� ������ �޾ƿ� ���̴�)
                InterlockedCompareExchange(SpawnCount, orgvalue, SpawnCount - 1, outvalue);
          
            // Interlocked �������� �� ��� ���������, Interlocked�� ���� ������ ��������� ������ �޾�
            // ���� ó���� �� �����常�� ���Կ� �����Ͽ� �Լ��� �����Ų��.
            // �׷��� 3���� ���� ��ü�� ���ټ����� ��� ���� �ѹ��� �����ϰ� �ο��ޱ� ������
            // ù �����带 ������ ��� ��������� outvalue���� �̹� ù �����尡 �ٿ����ұ� ������ SpawnCount -1�� ���̴�?
            // ex. SpawnCount�� 3�� ���, 100 ������ �� ���� 3 �����尡 ����ϴ� ���� �ƴ϶�,
            // 100�� �� 1�� ������, 99�� �� 1�� ������, 98�� �� 1�� ������.. ó�� ������ 3�� �ϴ� ��
            
            // ���� ����: Org = Out�� ���
                if (orgvalue == outvalue)
                {
                    // ��ƼŬ Ȱ��ȭ
                    particle.Active = 1;
                    
                    // ��ƼŬ ���� ����
                    particle.Mass = 1.f;
                    
                    // 0. ���� ����
                    float3 vOut1 = (float3) 0.f;
                    float3 vOut2 = (float3) 0.f;
                    float3 vOut3 = (float3) 0.f;
                    
                    // ��ü ��ȿ �������� ���̵� 0 ~ 1 �� ����ȭ
                    float fNormalizeThreadID = (float) _ID.x / (float) ParticleMaxCount;
                    GaussianSample(NoiseTexture, NoiseTexResolution, fNormalizeThreadID, vOut1);
                    GaussianSample(NoiseTexture, NoiseTexResolution, fNormalizeThreadID + 0.1f, vOut2);
                    GaussianSample(NoiseTexture, NoiseTexResolution, fNormalizeThreadID + 0.2f, vOut3);
                    
                    // 1. ���� ����
                    if (ModuleData.SpawnShapeType == 0)     // Box ����
                    {
                        particle.vLocalPos.xyz = float3(ModuleData.vBoxShapeScale.x * vOut1.r - ModuleData.vBoxShapeScale.x * 0.5f
                                                      , ModuleData.vBoxShapeScale.y * vOut2.r - ModuleData.vBoxShapeScale.y * 0.5f
                                                      , 0.f); //ModuleData.vBoxShapeScale.z * vOut3.r - ModuleData.vBoxShapeScale.z * 0.5f);
                        particle.vWorldPos.xyz = particle.vLocalPos.xyz + ObjectPos.xyz;
                        
                        // ���� �ּ�~�ִ� ũ�� ���������� ��ƼŬ ũ�� �������� ���� (Min, Max �� ��ġ�ϸ� ����ũ��)
                        float4 vSpawnScale = ModuleData.vSpawnScaleMin + (ModuleData.vSpawnScaleMax - ModuleData.vSpawnScaleMin) * vOut3.x;
                        particle.vWorldScale.xyz = vSpawnScale.xyz;
                    }
                    else if (ModuleData.SpawnShapeType == 1)  // Sphere ����
                    {
                        float fRadius = ModuleData.fSphereShapeRadius;
                        float fAngle = vOut2.r * 2 * 3.1415926535f;
                        particle.vWorldPos.xyz = float3(fRadius * cos(fAngle), fRadius * sin(fAngle), 100.f);
                    }
                    
                    // 2. AddVelocity ���
                    if (ModuleData.AddVelocity)
                    {
                        if (ModuleData.AddVelocityType == 0)      // 0. From Center
                        {
                            // Velocity = ������ ������������ ��ֶ������� ���� ���Ⱚ(���� 1) * Speed
                            float3 vVelocity = normalize(particle.vLocalPos.xyz);
                            particle.vVelocity.xyz = vVelocity * ModuleData.Speed;
                        }
                        else if (ModuleData.AddVelocityType == 1) // 1. To Center
                        {
                            float3 vVelocity = normalize(particle.vLocalPos.xyz);
                            particle.vVelocity.xyz = -vVelocity * ModuleData.Speed;
                        }
                        else                                      // 2. ���� ����(Fixed Direction)
                        {
                            float3 vVelocity = normalize(ModuleData.vVelocityDir.xyz);
                            particle.vVelocity.xyz = vVelocity * ModuleData.Speed;
                        }
                    }

                    particle.vColor = ModuleData.vSpawnColor;
                    
                    particle.Age = 0.f;
                    particle.LifeTime = ModuleData.MinLifeTime + (ModuleData.MaxLifeTime - ModuleData.MinLifeTime) * vOut2.r;
                    break;
                }
            }
        }
    }
     // ��������� ����/���� ���ο� ���� ���� ��ƼŬ�� Ȱ��ȭ�� ���
    if (particle.Active)
    {
        // 0. ���������� �� �ʱ�ȭ
        particle.vForce.xyz = (float3) 0.f;
     
        // 1. ��ƼŬ Age ����, ����ȭ�� Age ����
        particle.PrevAge = particle.Age;
        particle.Age += g_DT;
        particle.NomalizedAge = saturate(particle.Age / particle.LifeTime);
        
        // 1-1. ��ƼŬ�� ������ ������, ��Ȱ��ȭ
        if (particle.LifeTime <= particle.Age)
        {
            particle.Active = 0.f;
        }
        
         // 2. NoiseForce(�������� ��) ���
        if (ModuleData.NoiseForce)
        {
            // �̹��� ó������ ���� ���� �޴� �Ŷ�� 
            if (particle.PrevAge == 0.f)
            {
                // ���� ����
                float3 vOut1 = (float3) 0.f;
                float3 vOut2 = (float3) 0.f;
                float3 vOut3 = (float3) 0.f;
                              
                float fNormalizeThreadID = (float) _ID.x / (float) ParticleMaxCount;
                GaussianSample(NoiseTexture, NoiseTexResolution, fNormalizeThreadID, vOut1);
                GaussianSample(NoiseTexture, NoiseTexResolution, fNormalizeThreadID + 0.1f, vOut2);
                GaussianSample(NoiseTexture, NoiseTexResolution, fNormalizeThreadID + 0.2f, vOut3);
                
                // ������ ������ ���� ���� �� ��ֶ����� �Ͽ� ���Ⱚ ���Ͽ� ���� ������
                float3 vForce = normalize(float3(vOut1.x, vOut2.x, vOut1.z));
                particle.vRandomForce.xyz = vForce * ModuleData.fNoiseForce;
            }
            else
            {
                // ��ƼŬ�� ���� Age�� ���� Age�� ���� NoiseTerm�� ������ ���� �� floor�� �Ҽ����� ������
                int Age = int(particle.Age * (1.f / ModuleData.fNoiseTerm));
                int PrevAge = int(particle.PrevAge * (1.f / ModuleData.fNoiseTerm));

                // �� ��(������)�� ���� ��쿡�� ���� ���ݿ� �ִٴ� ��, �ٸ� ��쿡�� ������ ������ �ΰ�
                // ���� �ٸ� ���ݿ� �����Ѵٴ� �� -> �� ���ݽð�(NoiseTerm)�� �Ѿ��ٴ� ��
                
                // ������ ������ �Ѿ ����, ���ο� ���� Force �� �ش�.
                if (Age != PrevAge)
                {
                    // ���� ����
                    float3 vOut1 = (float3) 0.f;
                    float3 vOut2 = (float3) 0.f;
                    float3 vOut3 = (float3) 0.f;
           
                    float fNormalizeThreadID = (float) _ID.x / (float) ParticleMaxCount;
                    GaussianSample(NoiseTexture, NoiseTexResolution, fNormalizeThreadID, vOut1);
                    GaussianSample(NoiseTexture, NoiseTexResolution, fNormalizeThreadID + 0.1f, vOut2);
                    GaussianSample(NoiseTexture, NoiseTexResolution, fNormalizeThreadID + 0.2f, vOut3);
                
                    // ������ ������ ���� ���� �� ��ֶ����� �Ͽ� ���Ⱚ ���Ͽ� ���� ������.
                    // ������ �״�� �������� ���� ���� 0~1 ���̸� ���� ���� �پ���� �ʱ� ������,
                    // -1 ~ 1 ������ ���� ���� �� �ֵ��� 2�� ���� �� 1�� ���ش�.
                    float3 vForce = normalize(float3(vOut1.x, vOut2.x, vOut1.z) * 2.f - 1.f);
                    particle.vRandomForce.xyz = vForce * ModuleData.fNoiseForce;
                }
            }
            
            // ��ƼŬ�� ���� �޴� ���� ������ ������ ���������� �����ش�.
            particle.vForce.xyz += particle.vRandomForce.xyz;
        }
       
        // ��ƼŬ�� ���� ���� �� ���, ���� ���� �ӵ��� ��ȭ�� ��� (F=MA)
        float3 vAccel = particle.vForce.xyz / particle.Mass; // ���ӵ�
        particle.vVelocity.xyz += vAccel * g_DT; 
        
        
         // �ӵ� ����(Drag) ���
        if (ModuleData.Drag)
        {
            // ��ƼŬ�� ���� �ӷ�
            float Speed = length(particle.vVelocity);
            float fDrag = ModuleData.StartDrag + (ModuleData.EndDrag - ModuleData.StartDrag) * particle.NomalizedAge;
            
            // �ӵ��� ������ �Ǿ� �ݴ�� �̵��ϴ� �� ����
            if (fDrag <= 0.f)
                fDrag = 0.001f;     // ���Ⱚ(Direction)�� ����� ���� 
            
            if (fDrag < Speed)  // �巡�� ������ �ӵ����� ū ���, �巡�׸�ŭ���� ������ �����Ѵ�
            {
                particle.vVelocity = normalize(particle.vVelocity) * fDrag;
            }
        }
        
       // �ӵ��� ���� ��ƼŬ��ġ �̵�
        if (ModuleData.Space == 0)      // ��µ� ���� ����
        {
            particle.vWorldPos += particle.vVelocity * g_DT;
        }
        else if (ModuleData.Space == 1) // �θ� ����ٴ�
        {
            particle.vLocalPos += particle.vVelocity * g_DT;
            particle.vWorldPos.xyz = particle.vLocalPos.xyz + ObjectPos.xyz;
        }
        
        // ũ�� ��ȭ ���
        if (ModuleData.ScaleChange)
            particle.ScaleFactor = ModuleData.StartScale + particle.NomalizedAge * (ModuleData.EndScale - ModuleData.StartScale);
        else
            particle.ScaleFactor = 1.f; // ��Ȱ��ȭ�� ������ ScaleFactor = 1 ���� (��ƼŬ�� ���� ũ��)
        
        // ���� ��ȭ ���
        if (ModuleData.ColorChange)
        {
            particle.vColor = ModuleData.vStartColor + particle.NomalizedAge * (ModuleData.vEndColor - ModuleData.vStartColor);
        }

    }
    
    // ��ƼŬ ������ ����
    ParticleBuffer[_ID.x] = particle;
}

#endif


/* GPU�� ����ó���� �ϱ� ������ ��� �����尡 ���ÿ� �۾��� �����ϴµ�,
���� ����ī��Ʈ(�������Ѿ��� ��ƼŬ�� ����)�� 1 �̻��̱⸸ �ϸ� ��� ��������� �� �ڱ� ��ƼŬ��
���� �����ϴٰ� �����Ͽ� ������ ��Ű�� ������ ��� ��ƼŬ�� ���� �����ȴ�.

�̷� ���� ������ ��ǻ�ͼ��̵��� ����ȭ ó���� �ʿ��ѵ�, �̶� ����ϴ� �Լ��� �ٷ� 
Atomic Function, ���ڴ��� �Լ��̴� (������ ���� ������ ����Ǵ� �Լ�) : InterlockedCompareExchange
�ƹ��� ����ó���� �ϴ��� �� �Լ��� ���� �Ǹ� GPU���� �ھ�� ���� �����ϴ� �����尡
���� ����� �� �ֵ��� ������ �Ű��ָ�, �� �����尡 ���� ����Ǳ� ������ �ٸ� �����尡 ������� ���Ѵ�. 


InterlockedExchange() �Լ��� ����ϸ� ������ ���� �ٲ������ ������ 
������ ���￡ ���������� ���ߴ� �����尡 ���� ���￡�� �켱������ �޾� ����ī��Ʈ�� Ƣ�� ������ �߻��� �� �ִ�.
���� InterlockedCompareExchange() �Լ��� ����Ͽ� �����ڸ� �ϳ� �� �ξ�
���� ���ڿ� ��ġ�� ������ ���� ��ü�ϰ� �Ǿ� ���ܻ�Ȳ�� �ذ��� �� �ִ�.*/


