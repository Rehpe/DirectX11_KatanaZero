#ifndef _FUNC
#define _FUNC

#include "value.fx"

void CalcLight2D(float3 _vWorldPos, inout tLightColor _Light)
{
    for (uint i = 0; i < g_Light2DCount; ++i)
    {
        // Directional Light(방향성 광원) -> 전역 광원(월드 전역에 동일한 방향으로 내리쬐는 광원)
        if (g_Light2DBuffer[i].LightType == 0)
        {
            // 받아온 광원 정보의 Diffuse값을 그대로 더해준다
            _Light.vDiffuse.rgb += g_Light2DBuffer[i].Color.vDiffuse.rgb;
            _Light.vAmbient.rgb += g_Light2DBuffer[i].Color.vAmbient.rgb;
        }
        // Point Light(포인트 광원)       -> 포인트 광원에는 Ambient가 없으므로 계산하지 않는다
        else if (g_Light2DBuffer[i].LightType == 1)
        {
            float3 vLightWorldPos = float3(g_Light2DBuffer[i].vWorldPos.xy, 0.f); // 광원 위치
            float3 vWorldPos = float3(_vWorldPos.xy, 0.f); // 픽셀 위치

            float fDistance = abs(distance(vWorldPos, vLightWorldPos)); // 광원과 픽셀 사이의 거리
            
            // 픽셀이 광원에 가까울수록 빛 영향을 많이 받고, 멀수록 적게 받는다
            // 거리가 반지름을 기준으로 광원에 가깝다면(0) 빛의 영향이 세고, 반지름에 가깝다면(1) 빛 영향이 사라진다.
            // 빛의 세기가 음수가 되거나 1을 넘으면 안되므로 saturate 함수로 0~1범위로 잡아준다.
            
            float fPow = saturate(1.f - (fDistance / g_Light2DBuffer[i].Radius));
        
            _Light.vDiffuse.rgb += g_Light2DBuffer[i].Color.vDiffuse.rgb * fPow;
        }
        // Spot Light(스포트 라이트)
        else if (g_Light2DBuffer[i].LightType == 2)
        {
            float3 vLightWorldPos = float3(g_Light2DBuffer[i].vWorldPos.xy, 0.f); // 광원 위치
            float3 vWorldPos = float3(_vWorldPos.xy, 0.f);                // 픽셀 위치
            
            float fDistance = abs(distance(vWorldPos, vLightWorldPos)); // 광원과 픽셀 사이의 거리 
            float fDistancePow = saturate(1.f - (fDistance / g_Light2DBuffer[i].Radius));
         
            // Cutoff Angle (광원이 지면으로 향하는 법선 ~ 빛이 향하는 각도) : 원하는 각도의 1 / 2
            float Angle = g_Light2DBuffer[i].Angle / 2.f;
            float AngleCos = cos(Angle);
                      
            float3 PixelDir = normalize(vWorldPos - vLightWorldPos); // 광원에서 픽셀을 향하는 단위벡터
            float3 LightDir = normalize(g_Light2DBuffer[i].vWorldDir.xyz); // 광원에서 수직으로 떨어지는 단위벡터, 빛의 방향
            
            // 광원벡터(L)와 광원-픽셀 벡터(P) 둘 다 정규화하면 길이값이 1이 되므로 내적 결과는 cos θ(L과 P의 사이값) 가 나온다.
            float PixelCos = dot(PixelDir, LightDir);
            
            // 스포트 라이트는 특정 각도만을 비추는 광원이므로, 특성상 Cutoff Angle이 90도를 넘을 수 없다.
            // 따라서 0~90도 사이의 코사인 그래프(1~0값을 가짐) 에서, cosθ(PixelCos)값이 AngleCos값보다 작다면 (0에 가깝다면) 
            // 광원의 영향 밖의 각도에 있다는 뜻으로, 아무 일도 일어나지 않는다.
            // 반대로 cosθ(PixelCos)값이 AngleCos보다 크다면(1에 가깝다면) 광원의 영향을 받는다는 뜻이다.
            
            // 광원의 영향권 밖
            if (PixelCos < AngleCos)
            {
                // 빛을 받지 못한다.
                _Light.vDiffuse.rgb += float4(0.f, 0.f, 0.f, 0.f);
            }
            // 광원의 영향권 안
            else
            {
                // 빛의 영향을 받는다.
                
                // 이때 광원의 방향(직각, 코사인값 1)이 광원의 세기가 가장 세고, Cutoff Angle 방향이 광원의 세기가 가장 약하다(0)
                // 광원의 세기를 부드럽게 보간하기 위하여, 빛의 세기인 fPow를 구하여 최종 빛의 세기에 곱해준다.
                
                // AngleCos ~ 1 사이의 값을 0 ~ 1 사이로 정규화하고, 그 사이의 PixelCos값이 해당하는 만큼을 곱해준다.
                float fAnglePow = (1.f - (1.f - PixelCos) / (1.f - AngleCos));
                
                _Light.vDiffuse.rgb += g_Light2DBuffer[i].Color.vDiffuse.rgb * fDistancePow * fAnglePow;
            }
        }
    }
}

// 노멀값이 있는 경우의 빛 세기 계산 함수
void CalcLight2D(float3 _vWorldPos, float3 _vWorldDir, inout tLightColor _Light)
{
    for (uint i = 0; i < g_Light2DCount; ++i)
    {
        // Directional Light(방향성 광원)
        if (g_Light2DBuffer[i].LightType == 0)
        {
            // 램버트-코사인 법칙에 따라, 빛이 들어오는 방향과 표면의 방향(normal)이 0도에 가까울수록 빛의 세기가 커진다.
            // 두 방향을 같게 만들기 위해 빛의 방향에 음수를 붙여준 뒤 내적한다.
            // 빛 - 표면방향이 90도를 넘어가면(빛을 등지고 있다면) 파워가 음수가 되어 기존의 빛을 뺏어가는 현상이 발생할 수 있으므로
            // 0이하는 0, 1이상은 1로 조정해주는 saturate 함수를 붙여준다.
            
            float fDiffusePow = saturate(dot(-g_Light2DBuffer[i].vWorldDir.xyz, _vWorldDir));
            _Light.vDiffuse.rgb += g_Light2DBuffer[i].Color.vDiffuse.rgb * fDiffusePow;
            _Light.vAmbient.rgb += g_Light2DBuffer[i].Color.vAmbient.rgb;
        }
        // Point Light (포인트 광원)
        else if (g_Light2DBuffer[i].LightType == 1)
        {
            // 포인트 광원의 경우 빛의 방향이 하나만 있는 것이 아니라 방향에서 뻗어나오는 방향이 다 다르다.
            // 광원 위치 - 픽셀 위치를 빼서 광원에서 픽셀을 향하는 방향을 알아낸 뒤, 방향성 광원처럼 음수를 붙이고 내적한 후 saturate한다.
            
            float3 vLightWorldPos = float3(g_Light2DBuffer[i].vWorldPos.xy, 0.f);
            float3 vWorldPos = float3(_vWorldPos.xy, 0.f);

            // 광원 중심에서 물체를 향하는 방향
            float3 vLight = normalize(vWorldPos - vLightWorldPos);
            float fDiffusePow = saturate(dot(-vLight, _vWorldDir));
            
            // Radius 값에 따라 빛의 세기를 조절한다.
            float fDistance = abs(distance(vWorldPos, vLightWorldPos));
            float fDistPow = saturate(1.f - (fDistance / g_Light2DBuffer[i].Radius));
        
            // 위에서 구한 빛의 방향에 따른 빛의 세기와 빛의 거리에 따른 빛의 세기를 각각 곱한 후 더해준다.
            _Light.vDiffuse.rgb += g_Light2DBuffer[i].Color.vDiffuse.rgb * fDiffusePow * fDistPow;
        }
        else if (g_Light2DBuffer[i].LightType == 2)
        {
            
        }
    }
}

// =========== Random ===============

// 시스템 메모리(CPU)에서 난수 발생기는 현재 누적 시간값을 시드로 하여 랜덤을 뽑았다.
// 그러나 우리는 ComputeShader, 즉 GPU에서 난수를 생성하려 한다.
// GPU에서는 모든 쓰레드가 병렬 처리되어 동시에 함수에 진입하게 되므로 모두 같은 시간값을 사용하게 되어
// 모든 쓰레드의 결과가 동일하게 될 것이다.
// 따라서 GPU에서의 랜덤은 노이즈텍스처에서 어떤 픽셀을 골라 주변값의 RGB값을 샘플링해오는 식으로 설계할 것이다.

static float GaussianFilter[5][5] =
{
    // 노이즈 텍스처의 RGB값을 보정해서 뽑아오기 위한 필터
    0.003f, 0.0133f, 0.0219f, 0.0133f, 0.003f,
    0.0133f, 0.0596f, 0.0983f, 0.0596f, 0.0133f,
    0.0219f, 0.0983f, 0.1621f, 0.0983f, 0.0219f,
    0.0133f, 0.0596f, 0.0983f, 0.0596f, 0.0133f,
    0.003f, 0.0133f, 0.0219f, 0.0133f, 0.003f,
};

void GaussianSample(in Texture2D _NoiseTex, float2 _vResolution, float _NomalizedThreadID, out float3 _vOut)
{
    // ID값으로 겹치지 않는 UV값을 만들어 픽셀을 샘플링해온다.
    // _NormalizedThreadID : 정규화된 ID값. 현재 ID / 파티클 최대 개수 (0~1)
    
    float2 vUV = float2(_NomalizedThreadID, 0.5f);
  
    // 랜덤이 고정되는 문제를 해결하기 위해, 누적시간값만큼 그래프를 이동시킨다.
    // 단, x값과 y값이 다르게 움직이게 하기 위해 y축은 누적시간만큼, x축은 누적시간 / 2만큼 움직이겠다.
  
    vUV.x += g_AccTime * 0.5f;
    
    // sin 그래프로 텍스쳐의 샘플링 위치 UV 를 계산
    vUV.y -= (sin((_NomalizedThreadID - (g_AccTime /*그래프 우측 이동 속도*/)) * 2.f * 3.1415926535f * 10.f /*반복주기*/) / 2.f);
  
    // x, y값이 0과 1 사이로 나오도록 보정
    if (1.f < vUV.x)
        vUV.x = frac(vUV.x);
    else if (vUV.x < 0.f)
        vUV.x = 1.f + frac(vUV.x);
    
    if (1.f < vUV.y)
        vUV.y = frac(vUV.y);
    else if (vUV.y < 0.f)
        vUV.y = 1.f + frac(vUV.y);
    
    int2 pixel = vUV * _vResolution;
    int2 offset = int2(-2, -2);     // 찍은 픽셀의 -2, -2 지점으로 이동한 뒤 그 지점을 기준 5x5픽셀을 긁어온다
    float3 vOut = (float3) 0.f;
    
    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            vOut += _NoiseTex[pixel + offset + int2(j, i)] * GaussianFilter[i][j];
        }
    }
    
    _vOut = vOut;
}


#endif
