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


// numsthread를 1차원 설정하는 이유?
// DispatchThreadID.x를 사용하여 각 ID값으로 파티클에 접근하기 위해
[numthreads(128, 1, 1)]
void CS_ParticleUpdate(int3 _ID : SV_DispatchThreadID)
{
    // 파티클 최대 개수를 넘어가면 더 이상 실행하지 않고 return
    if (ParticleMaxCount <= _ID.x)
        return;
       
    tParticle particle = ParticleBuffer[_ID.x];
    
    if (SpawnModule) // 스폰 모듈 활성화시
    {
     // 파티클이 비활성화 상태인 경우
        if (particle.Active == 0)
        {
        // SpawnCount 가 0 이상이라면, 파티클을 활성화시킴      
            while (0 < SpawnCount)
            {
                int orgvalue = SpawnCount;
                int outvalue = 0;
            
            // 인자1: 값이 변경될 목적지 (파티클을 하나 생성하고 SpawnCount에서 -1을 해줘야하므로)
            // 인자2: 목적지의 값과 비교할 값. 인자1 = 인자2 인 경우에만 인자1을 인자3값으로 교체한다.
            // 인자3: Value(목적지의 값과 교환할 값) : SpawnCount - 1
            // 인자4: Out(레퍼런스, 결과를 담아주는 변수) : original, 목적지에 원래 들어있던 int값 
            // (원래 SpawnCount에 들어있던 값을 해당 변수에 받아올 것이다)
                InterlockedCompareExchange(SpawnCount, orgvalue, SpawnCount - 1, outvalue);
          
            // Interlocked 직전까지 온 모든 쓰레드들이, Interlocked에 먼저 접근한 쓰레드부터 순서를 받아
            // 제일 처음에 온 쓰레드만이 진입에 성공하여 함수를 실행시킨다.
            // 그러나 3개의 인자 자체는 접근순서와 상관 없이 한번에 동일하게 부여받기 때문에
            // 첫 쓰레드를 제외한 모든 쓰레드들은 outvalue값이 이미 첫 쓰레드가 줄여놓았기 때문에 SpawnCount -1일 것이다?
            // ex. SpawnCount가 3일 경우, 100 쓰레드 중 선착 3 쓰레드가 통과하는 것이 아니라,
            // 100명 중 1등 쓰레드, 99명 중 1등 쓰레드, 98명 중 1등 쓰레드.. 처럼 경쟁을 3번 하는 것
            
            // 성공 조건: Org = Out일 경우
                if (orgvalue == outvalue)
                {
                    // 파티클 활성화
                    particle.Active = 1;
                    
                    // 파티클 질량 설정
                    particle.Mass = 1.f;
                    
                    // 0. 난수 생성
                    float3 vOut1 = (float3) 0.f;
                    float3 vOut2 = (float3) 0.f;
                    float3 vOut3 = (float3) 0.f;
                    
                    // 전체 유효 스레드의 아이디를 0 ~ 1 로 정규화
                    float fNormalizeThreadID = (float) _ID.x / (float) ParticleMaxCount;
                    GaussianSample(NoiseTexture, NoiseTexResolution, fNormalizeThreadID, vOut1);
                    GaussianSample(NoiseTexture, NoiseTexResolution, fNormalizeThreadID + 0.1f, vOut2);
                    GaussianSample(NoiseTexture, NoiseTexResolution, fNormalizeThreadID + 0.2f, vOut3);
                    
                    // 1. 스폰 영역
                    if (ModuleData.SpawnShapeType == 0)     // Box 스폰
                    {
                        particle.vLocalPos.xyz = float3(ModuleData.vBoxShapeScale.x * vOut1.r - ModuleData.vBoxShapeScale.x * 0.5f
                                                      , ModuleData.vBoxShapeScale.y * vOut2.r - ModuleData.vBoxShapeScale.y * 0.5f
                                                      , 0.f); //ModuleData.vBoxShapeScale.z * vOut3.r - ModuleData.vBoxShapeScale.z * 0.5f);
                        particle.vWorldPos.xyz = particle.vLocalPos.xyz + ObjectPos.xyz;
                        
                        // 스폰 최소~최대 크기 범위내에서 파티클 크기 랜덤으로 지정 (Min, Max 가 일치하면 고정크기)
                        float4 vSpawnScale = ModuleData.vSpawnScaleMin + (ModuleData.vSpawnScaleMax - ModuleData.vSpawnScaleMin) * vOut3.x;
                        particle.vWorldScale.xyz = vSpawnScale.xyz;
                    }
                    else if (ModuleData.SpawnShapeType == 1)  // Sphere 스폰
                    {
                        float fRadius = ModuleData.fSphereShapeRadius;
                        float fAngle = vOut2.r * 2 * 3.1415926535f;
                        particle.vWorldPos.xyz = float3(fRadius * cos(fAngle), fRadius * sin(fAngle), 100.f);
                    }
                    
                    // 2. AddVelocity 모듈
                    if (ModuleData.AddVelocity)
                    {
                        if (ModuleData.AddVelocityType == 0)      // 0. From Center
                        {
                            // Velocity = 본인의 로컬포지션을 노멀라이즈한 순수 방향값(길이 1) * Speed
                            float3 vVelocity = normalize(particle.vLocalPos.xyz);
                            particle.vVelocity.xyz = vVelocity * ModuleData.Speed;
                        }
                        else if (ModuleData.AddVelocityType == 1) // 1. To Center
                        {
                            float3 vVelocity = normalize(particle.vLocalPos.xyz);
                            particle.vVelocity.xyz = -vVelocity * ModuleData.Speed;
                        }
                        else                                      // 2. 고정 방향(Fixed Direction)
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
     // 스폰모듈의 켜짐/꺼짐 여부와 관계 없이 파티클이 활성화인 경우
    if (particle.Active)
    {
        // 0. 이전프레임 힘 초기화
        particle.vForce.xyz = (float3) 0.f;
     
        // 1. 파티클 Age 누적, 정규화된 Age 구함
        particle.PrevAge = particle.Age;
        particle.Age += g_DT;
        particle.NomalizedAge = saturate(particle.Age / particle.LifeTime);
        
        // 1-1. 파티클의 수명이 끝나면, 비활성화
        if (particle.LifeTime <= particle.Age)
        {
            particle.Active = 0.f;
        }
        
         // 2. NoiseForce(랜덤방향 힘) 모듈
        if (ModuleData.NoiseForce)
        {
            // 이번에 처음으로 랜덤 힘을 받는 거라면 
            if (particle.PrevAge == 0.f)
            {
                // 난수 생성
                float3 vOut1 = (float3) 0.f;
                float3 vOut2 = (float3) 0.f;
                float3 vOut3 = (float3) 0.f;
                              
                float fNormalizeThreadID = (float) _ID.x / (float) ParticleMaxCount;
                GaussianSample(NoiseTexture, NoiseTexResolution, fNormalizeThreadID, vOut1);
                GaussianSample(NoiseTexture, NoiseTexResolution, fNormalizeThreadID + 0.1f, vOut2);
                GaussianSample(NoiseTexture, NoiseTexResolution, fNormalizeThreadID + 0.2f, vOut3);
                
                // 구해진 난수로 벡터 생성 후 노멀라이즈 하여 방향값 구하여 힘을 곱해줌
                float3 vForce = normalize(float3(vOut1.x, vOut2.x, vOut1.z));
                particle.vRandomForce.xyz = vForce * ModuleData.fNoiseForce;
            }
            else
            {
                // 파티클의 현재 Age와 이전 Age에 각각 NoiseTerm의 역수를 곱한 뒤 floor로 소수점을 버린다
                int Age = int(particle.Age * (1.f / ModuleData.fNoiseTerm));
                int PrevAge = int(particle.PrevAge * (1.f / ModuleData.fNoiseTerm));

                // 두 값(정수부)이 같은 경우에는 같은 간격에 있다는 것, 다른 경우에는 지정한 간격을 두고
                // 서로 다른 간격에 존재한다는 뜻 -> 즉 간격시간(NoiseTerm)을 넘었다는 뜻
                
                // 지정한 간격을 넘어간 순간, 새로운 랜덤 Force 를 준다.
                if (Age != PrevAge)
                {
                    // 난수 생성
                    float3 vOut1 = (float3) 0.f;
                    float3 vOut2 = (float3) 0.f;
                    float3 vOut3 = (float3) 0.f;
           
                    float fNormalizeThreadID = (float) _ID.x / (float) ParticleMaxCount;
                    GaussianSample(NoiseTexture, NoiseTexResolution, fNormalizeThreadID, vOut1);
                    GaussianSample(NoiseTexture, NoiseTexResolution, fNormalizeThreadID + 0.1f, vOut2);
                    GaussianSample(NoiseTexture, NoiseTexResolution, fNormalizeThreadID + 0.2f, vOut3);
                
                    // 구해진 난수로 벡터 생성 후 노멀라이즈 하여 방향값 구하여 힘을 곱해줌.
                    // 난수를 그대로 가져오면 힘의 값이 0~1 사이만 나와 힘이 줄어들지 않기 때문에,
                    // -1 ~ 1 사이의 값이 나올 수 있도록 2를 곱한 후 1을 빼준다.
                    float3 vForce = normalize(float3(vOut1.x, vOut2.x, vOut1.z) * 2.f - 1.f);
                    particle.vRandomForce.xyz = vForce * ModuleData.fNoiseForce;
                }
            }
            
            // 파티클이 현재 받는 힘에 위에서 구해진 랜덤포스를 더해준다.
            particle.vForce.xyz += particle.vRandomForce.xyz;
        }
       
        // 파티클에 힘이 적용 된 경우, 힘에 의한 속도의 변화량 계산 (F=MA)
        float3 vAccel = particle.vForce.xyz / particle.Mass; // 가속도
        particle.vVelocity.xyz += vAccel * g_DT; 
        
        
         // 속도 제한(Drag) 모듈
        if (ModuleData.Drag)
        {
            // 파티클의 현재 속력
            float Speed = length(particle.vVelocity);
            float fDrag = ModuleData.StartDrag + (ModuleData.EndDrag - ModuleData.StartDrag) * particle.NomalizedAge;
            
            // 속도가 음수가 되어 반대로 이동하는 것 방지
            if (fDrag <= 0.f)
                fDrag = 0.001f;     // 방향값(Direction)을 남기기 위해 
            
            if (fDrag < Speed)  // 드래그 값보다 속도값이 큰 경우, 드래그만큼으로 강제로 제한한다
            {
                particle.vVelocity = normalize(particle.vVelocity) * fDrag;
            }
        }
        
       // 속도에 따른 파티클위치 이동
        if (ModuleData.Space == 0)      // 출력된 곳에 고정
        {
            particle.vWorldPos += particle.vVelocity * g_DT;
        }
        else if (ModuleData.Space == 1) // 부모를 따라다님
        {
            particle.vLocalPos += particle.vVelocity * g_DT;
            particle.vWorldPos.xyz = particle.vLocalPos.xyz + ObjectPos.xyz;
        }
        
        // 크기 변화 모듈
        if (ModuleData.ScaleChange)
            particle.ScaleFactor = ModuleData.StartScale + particle.NomalizedAge * (ModuleData.EndScale - ModuleData.StartScale);
        else
            particle.ScaleFactor = 1.f; // 비활성화시 언제나 ScaleFactor = 1 고정 (파티클의 원본 크기)
        
        // 색상 변화 모듈
        if (ModuleData.ColorChange)
        {
            particle.vColor = ModuleData.vStartColor + particle.NomalizedAge * (ModuleData.vEndColor - ModuleData.vStartColor);
        }

    }
    
    // 파티클 변경점 적용
    ParticleBuffer[_ID.x] = particle;
}

#endif


/* GPU는 병렬처리를 하기 때문에 모든 쓰레드가 동시에 작업을 수행하는데,
따라서 스폰카운트(스폰시켜야할 파티클의 개수)가 1 이상이기만 하면 모든 쓰레드들이 다 자기 파티클이
스폰 가능하다고 생각하여 스폰을 시키기 때문에 모든 파티클이 전부 스폰된다.

이런 이유 때문에 컴퓨터셰이드의 동기화 처리가 필요한데, 이때 사용하는 함수가 바로 
Atomic Function, 원자단위 함수이다 (쓰레드 별로 접근이 보장되는 함수) : InterlockedCompareExchange
아무리 병렬처리를 하더라도 이 함수를 쓰게 되면 GPU에서 코어별로 먼저 접근하는 쓰레드가
먼저 수행될 수 있도록 순서를 매겨주며, 한 쓰레드가 먼저 실행되기 전에는 다른 쓰레드가 실행되지 못한다. 


InterlockedExchange() 함수를 사용하면 무조건 값을 바꿔버리기 때문에 
쓰레드 경쟁에 참여하지도 못했던 쓰레드가 다음 경쟁에서 우선순위를 받아 스폰카운트가 튀는 현상이 발생할 수 있다.
따라서 InterlockedCompareExchange() 함수를 사용하여 비교인자를 하나 더 두어
비교할 인자와 일치할 때에만 값을 교체하게 되어 예외상황을 해결할 수 있다.*/


