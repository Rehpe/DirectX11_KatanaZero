#ifndef _STD2D
#define _STD2D

// ���� �������� ������ ��Ƴ��� value.fx ��Ŭ���
#include "value.fx"

// Lighting (�� ���� ���) �Լ�
#include "func.fx"

// �Ϲ����� 2D �������� ���� STD2D ���̴�

struct VS_IN
{
    float3 vLocalPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

// =======================================
// Std2D Shader
// RasterizerState      : None
// BlendState           : Mask
// DepthStencilState    : Less
//
// Parameter
// g_int_0              : AnimUse (�ִϸ��̼� ��� ����)
// g_vec2_0             : AnimAtlas LeftTop
// g_vec2_1             : AnimAtlas Slice

// g_tex_0              : Output Texture
// ========================================
VS_OUT VS_Std2D_Alpha(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f; // HLSL ����ü �ʱ�ȭ
    
   // ������ǥ�� 1�� Ȯ����(��ǥ) ���� Pos�� ����*��*������ ���س��� matWVP �ѹ��� ���Ͽ� ���� ������� ��ȯ
    output.vPosition = mul(float4(_in.vLocalPos, 1.f), g_matWVP);
    output.vUV = _in.vUV; // UV���� �״�� ����
    
    return output;
}

float4 PS_Std2D_Alpha(VS_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f; // �ʱ�ȭ
    
    if (g_btex_0)        // �ؽ�ó ���ε��� ����� �Ǿ��ٸ�(bool���� 1�̶��) �ؽ�ó ���
    {
        if(g_float_0 != 0)
        {
            vOutColor = g_tex_0.Sample(g_sam_1, _in.vUV);
        }
        float alpha = g_float_0;
        vOutColor = float4(vOutColor.rgb, alpha);
    }
    else
    {
        float alpha = g_float_0;
        vOutColor = float4(0, 0, 0, alpha); 
        //vOutColor = float4(1.f, 1.f, 0.f, 1.f);
    }
    
    return vOutColor;
}

VS_OUT VS_Std2D_Laser(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f; // HLSL ����ü �ʱ�ȭ
    
   // ������ǥ�� 1�� Ȯ����(��ǥ) ���� Pos�� ����*��*������ ���س��� matWVP �ѹ��� ���Ͽ� ���� ������� ��ȯ
    output.vPosition = mul(float4(_in.vLocalPos, 1.f), g_matWVP);
    output.vUV = _in.vUV; // UV���� �״�� ����
    
    return output;
}

float4 PS_Std2D_Laser(VS_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f; // �ʱ�ȭ
    
    float2 uvOffset = float2(0.0, g_AccTime * -0.5); 
    
    float4 redDots = g_tex_0.Sample(g_sam_1, _in.vUV + uvOffset);
    float4 Magenta = g_tex_1.Sample(g_sam_1, _in.vUV);
    
    float mask = Magenta.r;
    
    vOutColor = redDots * mask;
    return vOutColor;
}

VS_OUT VS_Std2D(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f; // HLSL ����ü �ʱ�ȭ
    
   // ������ǥ�� 1�� Ȯ����(��ǥ) ���� Pos�� ����*��*������ ���س��� matWVP �ѹ��� ���Ͽ� ���� ������� ��ȯ
    output.vPosition = mul(float4(_in.vLocalPos, 1.f), g_matWVP);
    output.vUV = _in.vUV; // UV���� �״�� ����
    
    return output;
}

float3 PS_Std2D(VS_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f; // �ʱ�ȭ
    
    // �ؽ�ó ���ε��� �Ǿ����� ���� �� �������� �õ��� ���, ������� ���� �ϱ� ���� �ش� ������ ����Ÿ�� ��µǰ� �� ���̴�.
    // �ؽ�ó �������Ϳ� �ؽ�ó�� ���������� ���ε��� �Ǿ������� ���θ� ���� ��� ���۷� �����Ͽ�,
    // �� ���� ���� ���ø� / ����Ÿ ��� ���θ� �����ϰ� �Ѵ�
    
    if (g_btex_0)        // �ؽ�ó ���ε��� ����� �Ǿ��ٸ�(bool���� 1�̶��) �ؽ�ó ���
    {
        vOutColor = g_tex_0.Sample(g_sam_1, _in.vUV);
    }
    else // �ؽ�ó ���ε��� �Ǿ����� �ʴٸ�(bool���� 0�̶��) �ƹ��͵� ������� ����
    {
        vOutColor = float4(0.f, 0.f, 0.f, 0.f);
    }
    
    // ���İ��� 0�� ��쿡�� ������.
    if (0.f == vOutColor.a)
        discard;
    
    return vOutColor;
}

// =======================================
// Std2D_UI Shader
// RasterizerState      : None
// BlendState           : Mask
// DepthStencilState    : Less
//
// Parameter
// g_int_0              : �پ��°� ��� ����(TimerMtrl�� ���)
// g_float_0            : TimePercentage

// g_tex_0              : Output Texture
// ========================================

VS_OUT VS_Std2D_UI(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f; // HLSL ����ü �ʱ�ȭ
    
   // ������ǥ�� 1�� Ȯ����(��ǥ) ���� Pos�� ����*��*������ ���س��� matWVP �ѹ��� ���Ͽ� ���� ������� ��ȯ
    output.vPosition = mul(float4(_in.vLocalPos, 1.f), g_matWVP);
    output.vUV = _in.vUV; // UV���� �״�� ����
    
    return output;
}

float3 PS_Std2D_UI(VS_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f; // �ʱ�ȭ
    
    if (g_btex_0)        // �ؽ�ó ���ε��� ����� �Ǿ��ٸ�(bool���� 1�̶��) �ؽ�ó ���
    {
        vOutColor = g_tex_0.Sample(g_sam_1, _in.vUV);
        
        if(g_int_0)
        {
            if (_in.vUV.x > g_float_0)
            {
                discard;
            }
        }
    }
    else // �ؽ�ó ���ε��� �Ǿ����� �ʴٸ�(bool���� 0�̶��) �ƹ��͵� ������� ����
    {
        vOutColor = float4(0.f, 0.f, 0.f, 0.f);
    }
    
    // ���İ��� 0�� ��쿡�� ������.
    if (0.f == vOutColor.a)
        discard;
    
    return vOutColor;
}


// ======================================
// Std2DLightShader
// RasterizerState      : None
// BlendState           : Mask
// DepthStencilState    : Less
//
// Parameter
// g_int_0              : AnimUse (�ִϸ��̼� ��� ����)
// g_vec2_0             : AnimAtlas LeftTop
// g_vec2_1             : AnimAtlas Slice
// g_vec2_2             : AnimAtlas Offset
// g_vec2_3             : AnimAtlas BackSize

#define bAnimUse         g_int_0
#define bColorOverlayUse g_int_1
#define bBecomeTransParent g_int_2
#define OverlayColor    g_vec4_0
#define OverlayAlpha    g_float_0
#define LeftTop         g_vec2_0
#define Slice           g_vec2_1
#define Offset          g_vec2_2
#define BackSize        g_vec2_3

// g_tex_0              : Output Texture
// g_tex_1              : Nomal Texture
// ======================================

struct VS_Light_IN
{
    float3 vLocalPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_Light_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    float3 vWorldPos : POSITION;
};

VS_Light_OUT VS_Std2DLight(VS_Light_IN _in)
{
    VS_Light_OUT output = (VS_Light_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vLocalPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    output.vWorldPos = mul(float4(_in.vLocalPos, 1.f), g_matWorld).xyz;
    
        
    return output;
}

float4 PS_Std2DLight(VS_Light_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
       
    if (g_btex_0)       // �ؽ�ó ���ε��� �� ���
    {
        // �ִϸ��̼� ����
        if (bAnimUse)
        {
            //float2 normalizedUV = (_in.vUV - LeftTop) / BackSize;
            //float2 vUV = LeftTop + (Slice * normalizedUV) - Offset;
            float2 vUV = LeftTop + (BackSize * _in.vUV);
            vUV -= ((BackSize - Slice) / 2.f);
            vUV -= Offset;
            
            if (LeftTop.x < vUV.x && vUV.x < LeftTop.x + Slice.x
                && LeftTop.y < vUV.y && vUV.y < LeftTop.y + Slice.y)
            {
                vOutColor = g_tex_0.Sample(g_sam_1, vUV);
            }
            else
            {
                discard;
            }
        }
        // �ִϸ��̼� ������� ���� ��
        else
        {
            // ���ε��� �ؽ�ó ������
            vOutColor = g_tex_0.Sample(g_sam_1, _in.vUV);
        }
    }
    else // ���ε��� �ؽ�ó�� ���� ��� ������� ����
    {
        vOutColor = float4(0.f, 0.f, 0.f, 0.f);
    }
    
    float3 vNormal = (float3) 0.f;
    
    // �븻 ���� �ؽ��İ� �ִ� ��� ��ְ��� ����Ͽ� �ݿ��Ѵ�.
    if (g_btex_1)
    {
        // Normal �� ����  (���ø��� Ÿ�� ���� 0~1 ������ �ȴ�)
        vNormal = g_tex_1.Sample(g_sam_1, _in.vUV);
        
        // 0 ~ 1 ������ -1 ~ 1 �� ���� (-1 : ���� ����� ���ݴ�, 1 : ���� ���ֺ��� ����)
        vNormal = (vNormal * 2.f) - 1.f;
        
        // NormalTexture ��ǥ��� y��� z ���� �ݴ�� �Ǿ��ִ�.
        float f = vNormal.y;
        vNormal.y = vNormal.z;
        vNormal.z = f;
        
        // Texture ���� ������ Normal ������ ����� ��ȯ��Ų��. (ȸ����Ʈ�� ������Ѿ� �ϹǷ�, ������ǥ�� 0, ��ֶ��������ش�.)
        vNormal = normalize(mul(float4(vNormal, 0.f), g_matWorld)).xyz;
    }
        
    if (0.f == vOutColor.a)
        discard;
    
    if (bColorOverlayUse > 0)
    {
        float4 _TintColor = OverlayColor * 3;
        vOutColor.rgb = _TintColor;
        vOutColor.a = OverlayAlpha;
    }
    
    if (bBecomeTransParent > 0)
    {
        float normalizedElapsedTime = fmod(g_AccTime, 0.1) / 0.1;
        float transparency = 1.0 - normalizedElapsedTime;
        vOutColor.a *= transparency;
    }
    
    // Lighting ó��
    tLightColor LightColor = (tLightColor) 0.f;
    
    // vNormal �� Zero Vector ��� (���� ��ְ��� ���ٸ�)
    if (dot(vNormal, vNormal) == 0.f)
    {
        CalcLight2D(_in.vWorldPos, LightColor);
    }
    else
    {
        CalcLight2D(_in.vWorldPos, vNormal, LightColor);
    }
    
    // �ݺ����� ���� ���ؿ� ���� vLightColor ���� ����ϰ��� �ϴ� �ȼ� ���� �����ش�.
    vOutColor.rgb *= (LightColor.vDiffuse.rgb + LightColor.vAmbient.rgb);
    
    return vOutColor;
}


VS_OUT VS_Std2D_Blood(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f; // HLSL ����ü �ʱ�ȭ
    
   // ������ǥ�� 1�� Ȯ����(��ǥ) ���� Pos�� ����*��*������ ���س��� matWVP �ѹ��� ���Ͽ� ���� ������� ��ȯ
    output.vPosition = mul(float4(_in.vLocalPos, 1.f), g_matWVP);
    output.vUV = _in.vUV; // UV���� �״�� ����
    
    return output;
}

float3 PS_Std2D_Blood(VS_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f; // �ʱ�ȭ
       
    if (g_btex_0)       // �ؽ�ó ���ε��� �� ���
    {
        // �ִϸ��̼� ����
        if (bAnimUse)
        {
            //float2 normalizedUV = (_in.vUV - LeftTop) / BackSize;
            //float2 vUV = LeftTop + (Slice * normalizedUV) - Offset;
            float2 vUV = LeftTop + (BackSize * _in.vUV);
            vUV -= ((BackSize - Slice) / 2.f);
            vUV -= Offset;
            
            if (LeftTop.x < vUV.x && vUV.x < LeftTop.x + Slice.x
                && LeftTop.y < vUV.y && vUV.y < LeftTop.y + Slice.y)
            {
                vOutColor = g_tex_0.Sample(g_sam_1, vUV);
            }
            else
            {
                discard;
            }
        }
        // �ִϸ��̼� ������� ���� ��
        else
        {
            // ���ε��� �ؽ�ó ������
            vOutColor = g_tex_0.Sample(g_sam_1, _in.vUV);
        }
    }
    else // ���ε��� �ؽ�ó�� ���� ��� ������� ����
    {
        vOutColor = float4(0.f, 0.f, 0.f, 0.f);
    }
    
    // ���İ��� 0�� ��쿡�� ������.
    if (0.05f >= vOutColor.a)
        discard;
    
    float4 _TintColor = float4(0.8f, 0.f, 0.f, 1.f);
    
    vOutColor.rgb *= _TintColor.rgb;
    
    return vOutColor;
}





#endif
