#ifndef _FUNC
#define _FUNC

#include "value.fx"

void CalcLight2D(float3 _vWorldPos, inout tLightColor _Light)
{
    for (uint i = 0; i < g_Light2DCount; ++i)
    {
        // Directional Light(���⼺ ����) -> ���� ����(���� ������ ������ �������� �����ش� ����)
        if (g_Light2DBuffer[i].LightType == 0)
        {
            // �޾ƿ� ���� ������ Diffuse���� �״�� �����ش�
            _Light.vDiffuse.rgb += g_Light2DBuffer[i].Color.vDiffuse.rgb;
            _Light.vAmbient.rgb += g_Light2DBuffer[i].Color.vAmbient.rgb;
        }
        // Point Light(����Ʈ ����)       -> ����Ʈ �������� Ambient�� �����Ƿ� ������� �ʴ´�
        else if (g_Light2DBuffer[i].LightType == 1)
        {
            float3 vLightWorldPos = float3(g_Light2DBuffer[i].vWorldPos.xy, 0.f); // ���� ��ġ
            float3 vWorldPos = float3(_vWorldPos.xy, 0.f); // �ȼ� ��ġ

            float fDistance = abs(distance(vWorldPos, vLightWorldPos)); // ������ �ȼ� ������ �Ÿ�
            
            // �ȼ��� ������ �������� �� ������ ���� �ް�, �ּ��� ���� �޴´�
            // �Ÿ��� �������� �������� ������ �����ٸ�(0) ���� ������ ����, �������� �����ٸ�(1) �� ������ �������.
            // ���� ���Ⱑ ������ �ǰų� 1�� ������ �ȵǹǷ� saturate �Լ��� 0~1������ ����ش�.
            
            float fPow = saturate(1.f - (fDistance / g_Light2DBuffer[i].Radius));
        
            _Light.vDiffuse.rgb += g_Light2DBuffer[i].Color.vDiffuse.rgb * fPow;
        }
        // Spot Light(����Ʈ ����Ʈ)
        else if (g_Light2DBuffer[i].LightType == 2)
        {
            float3 vLightWorldPos = float3(g_Light2DBuffer[i].vWorldPos.xy, 0.f); // ���� ��ġ
            float3 vWorldPos = float3(_vWorldPos.xy, 0.f);                // �ȼ� ��ġ
            
            float fDistance = abs(distance(vWorldPos, vLightWorldPos)); // ������ �ȼ� ������ �Ÿ� 
            float fDistancePow = saturate(1.f - (fDistance / g_Light2DBuffer[i].Radius));
         
            // Cutoff Angle (������ �������� ���ϴ� ���� ~ ���� ���ϴ� ����) : ���ϴ� ������ 1 / 2
            float Angle = g_Light2DBuffer[i].Angle / 2.f;
            float AngleCos = cos(Angle);
                      
            float3 PixelDir = normalize(vWorldPos - vLightWorldPos); // �������� �ȼ��� ���ϴ� ��������
            float3 LightDir = normalize(g_Light2DBuffer[i].vWorldDir.xyz); // �������� �������� �������� ��������, ���� ����
            
            // ��������(L)�� ����-�ȼ� ����(P) �� �� ����ȭ�ϸ� ���̰��� 1�� �ǹǷ� ���� ����� cos ��(L�� P�� ���̰�) �� ���´�.
            float PixelCos = dot(PixelDir, LightDir);
            
            // ����Ʈ ����Ʈ�� Ư�� �������� ���ߴ� �����̹Ƿ�, Ư���� Cutoff Angle�� 90���� ���� �� ����.
            // ���� 0~90�� ������ �ڻ��� �׷���(1~0���� ����) ����, cos��(PixelCos)���� AngleCos������ �۴ٸ� (0�� �����ٸ�) 
            // ������ ���� ���� ������ �ִٴ� ������, �ƹ� �ϵ� �Ͼ�� �ʴ´�.
            // �ݴ�� cos��(PixelCos)���� AngleCos���� ũ�ٸ�(1�� �����ٸ�) ������ ������ �޴´ٴ� ���̴�.
            
            // ������ ����� ��
            if (PixelCos < AngleCos)
            {
                // ���� ���� ���Ѵ�.
                _Light.vDiffuse.rgb += float4(0.f, 0.f, 0.f, 0.f);
            }
            // ������ ����� ��
            else
            {
                // ���� ������ �޴´�.
                
                // �̶� ������ ����(����, �ڻ��ΰ� 1)�� ������ ���Ⱑ ���� ����, Cutoff Angle ������ ������ ���Ⱑ ���� ���ϴ�(0)
                // ������ ���⸦ �ε巴�� �����ϱ� ���Ͽ�, ���� ������ fPow�� ���Ͽ� ���� ���� ���⿡ �����ش�.
                
                // AngleCos ~ 1 ������ ���� 0 ~ 1 ���̷� ����ȭ�ϰ�, �� ������ PixelCos���� �ش��ϴ� ��ŭ�� �����ش�.
                float fAnglePow = (1.f - (1.f - PixelCos) / (1.f - AngleCos));
                
                _Light.vDiffuse.rgb += g_Light2DBuffer[i].Color.vDiffuse.rgb * fDistancePow * fAnglePow;
            }
        }
    }
}

// ��ְ��� �ִ� ����� �� ���� ��� �Լ�
void CalcLight2D(float3 _vWorldPos, float3 _vWorldDir, inout tLightColor _Light)
{
    for (uint i = 0; i < g_Light2DCount; ++i)
    {
        // Directional Light(���⼺ ����)
        if (g_Light2DBuffer[i].LightType == 0)
        {
            // ����Ʈ-�ڻ��� ��Ģ�� ����, ���� ������ ����� ǥ���� ����(normal)�� 0���� �������� ���� ���Ⱑ Ŀ����.
            // �� ������ ���� ����� ���� ���� ���⿡ ������ �ٿ��� �� �����Ѵ�.
            // �� - ǥ������� 90���� �Ѿ��(���� ������ �ִٸ�) �Ŀ��� ������ �Ǿ� ������ ���� ����� ������ �߻��� �� �����Ƿ�
            // 0���ϴ� 0, 1�̻��� 1�� �������ִ� saturate �Լ��� �ٿ��ش�.
            
            float fDiffusePow = saturate(dot(-g_Light2DBuffer[i].vWorldDir.xyz, _vWorldDir));
            _Light.vDiffuse.rgb += g_Light2DBuffer[i].Color.vDiffuse.rgb * fDiffusePow;
            _Light.vAmbient.rgb += g_Light2DBuffer[i].Color.vAmbient.rgb;
        }
        // Point Light (����Ʈ ����)
        else if (g_Light2DBuffer[i].LightType == 1)
        {
            // ����Ʈ ������ ��� ���� ������ �ϳ��� �ִ� ���� �ƴ϶� ���⿡�� ������� ������ �� �ٸ���.
            // ���� ��ġ - �ȼ� ��ġ�� ���� �������� �ȼ��� ���ϴ� ������ �˾Ƴ� ��, ���⼺ ����ó�� ������ ���̰� ������ �� saturate�Ѵ�.
            
            float3 vLightWorldPos = float3(g_Light2DBuffer[i].vWorldPos.xy, 0.f);
            float3 vWorldPos = float3(_vWorldPos.xy, 0.f);

            // ���� �߽ɿ��� ��ü�� ���ϴ� ����
            float3 vLight = normalize(vWorldPos - vLightWorldPos);
            float fDiffusePow = saturate(dot(-vLight, _vWorldDir));
            
            // Radius ���� ���� ���� ���⸦ �����Ѵ�.
            float fDistance = abs(distance(vWorldPos, vLightWorldPos));
            float fDistPow = saturate(1.f - (fDistance / g_Light2DBuffer[i].Radius));
        
            // ������ ���� ���� ���⿡ ���� ���� ����� ���� �Ÿ��� ���� ���� ���⸦ ���� ���� �� �����ش�.
            _Light.vDiffuse.rgb += g_Light2DBuffer[i].Color.vDiffuse.rgb * fDiffusePow * fDistPow;
        }
        else if (g_Light2DBuffer[i].LightType == 2)
        {
            
        }
    }
}

// =========== Random ===============

// �ý��� �޸�(CPU)���� ���� �߻���� ���� ���� �ð����� �õ�� �Ͽ� ������ �̾Ҵ�.
// �׷��� �츮�� ComputeShader, �� GPU���� ������ �����Ϸ� �Ѵ�.
// GPU������ ��� �����尡 ���� ó���Ǿ� ���ÿ� �Լ��� �����ϰ� �ǹǷ� ��� ���� �ð����� ����ϰ� �Ǿ�
// ��� �������� ����� �����ϰ� �� ���̴�.
// ���� GPU������ ������ �������ؽ�ó���� � �ȼ��� ��� �ֺ����� RGB���� ���ø��ؿ��� ������ ������ ���̴�.

static float GaussianFilter[5][5] =
{
    // ������ �ؽ�ó�� RGB���� �����ؼ� �̾ƿ��� ���� ����
    0.003f, 0.0133f, 0.0219f, 0.0133f, 0.003f,
    0.0133f, 0.0596f, 0.0983f, 0.0596f, 0.0133f,
    0.0219f, 0.0983f, 0.1621f, 0.0983f, 0.0219f,
    0.0133f, 0.0596f, 0.0983f, 0.0596f, 0.0133f,
    0.003f, 0.0133f, 0.0219f, 0.0133f, 0.003f,
};

void GaussianSample(in Texture2D _NoiseTex, float2 _vResolution, float _NomalizedThreadID, out float3 _vOut)
{
    // ID������ ��ġ�� �ʴ� UV���� ����� �ȼ��� ���ø��ؿ´�.
    // _NormalizedThreadID : ����ȭ�� ID��. ���� ID / ��ƼŬ �ִ� ���� (0~1)
    
    float2 vUV = float2(_NomalizedThreadID, 0.5f);
  
    // ������ �����Ǵ� ������ �ذ��ϱ� ����, �����ð�����ŭ �׷����� �̵���Ų��.
    // ��, x���� y���� �ٸ��� �����̰� �ϱ� ���� y���� �����ð���ŭ, x���� �����ð� / 2��ŭ �����̰ڴ�.
  
    vUV.x += g_AccTime * 0.5f;
    
    // sin �׷����� �ؽ����� ���ø� ��ġ UV �� ���
    vUV.y -= (sin((_NomalizedThreadID - (g_AccTime /*�׷��� ���� �̵� �ӵ�*/)) * 2.f * 3.1415926535f * 10.f /*�ݺ��ֱ�*/) / 2.f);
  
    // x, y���� 0�� 1 ���̷� �������� ����
    if (1.f < vUV.x)
        vUV.x = frac(vUV.x);
    else if (vUV.x < 0.f)
        vUV.x = 1.f + frac(vUV.x);
    
    if (1.f < vUV.y)
        vUV.y = frac(vUV.y);
    else if (vUV.y < 0.f)
        vUV.y = 1.f + frac(vUV.y);
    
    int2 pixel = vUV * _vResolution;
    int2 offset = int2(-2, -2);     // ���� �ȼ��� -2, -2 �������� �̵��� �� �� ������ ���� 5x5�ȼ��� �ܾ�´�
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
