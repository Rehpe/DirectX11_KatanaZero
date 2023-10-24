#ifndef _STD2D
#define _STD2D

// 전역 레지스터 변수만 모아놓은 value.fx 인클루드
#include "value.fx"

// Lighting (빛 세기 계산) 함수
#include "func.fx"

// 일반적인 2D 렌더링에 사용될 STD2D 셰이더

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
// g_int_0              : AnimUse (애니메이션 사용 여부)
// g_vec2_0             : AnimAtlas LeftTop
// g_vec2_1             : AnimAtlas Slice

// g_tex_0              : Output Texture
// ========================================
VS_OUT VS_Std2D_Alpha(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f; // HLSL 구조체 초기화
    
   // 동차좌표를 1로 확장한(좌표) 로컬 Pos에 월드*뷰*투영을 곱해놓은 matWVP 한번에 곱하여 최종 투영행렬 반환
    output.vPosition = mul(float4(_in.vLocalPos, 1.f), g_matWVP);
    output.vUV = _in.vUV; // UV값은 그대로 전달
    
    return output;
}

float4 PS_Std2D_Alpha(VS_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f; // 초기화
    
    if (g_btex_0)        // 텍스처 바인딩이 제대로 되었다면(bool값이 1이라면) 텍스처 출력
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
    VS_OUT output = (VS_OUT) 0.f; // HLSL 구조체 초기화
    
   // 동차좌표를 1로 확장한(좌표) 로컬 Pos에 월드*뷰*투영을 곱해놓은 matWVP 한번에 곱하여 최종 투영행렬 반환
    output.vPosition = mul(float4(_in.vLocalPos, 1.f), g_matWVP);
    output.vUV = _in.vUV; // UV값은 그대로 전달
    
    return output;
}

float4 PS_Std2D_Laser(VS_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f; // 초기화
    
    float2 uvOffset = float2(0.0, g_AccTime * -0.5); 
    
    float4 redDots = g_tex_0.Sample(g_sam_1, _in.vUV + uvOffset);
    float4 Magenta = g_tex_1.Sample(g_sam_1, _in.vUV);
    
    float mask = Magenta.r;
    
    vOutColor = redDots * mask;
    return vOutColor;
}

VS_OUT VS_Std2D(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f; // HLSL 구조체 초기화
    
   // 동차좌표를 1로 확장한(좌표) 로컬 Pos에 월드*뷰*투영을 곱해놓은 matWVP 한번에 곱하여 최종 투영행렬 반환
    output.vPosition = mul(float4(_in.vLocalPos, 1.f), g_matWVP);
    output.vUV = _in.vUV; // UV값은 그대로 전달
    
    return output;
}

float3 PS_Std2D(VS_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f; // 초기화
    
    // 텍스처 바인딩이 되어있지 않을 때 렌더링을 시도한 경우, 디버깅을 쉽게 하기 위해 해당 영역이 마젠타로 출력되게 할 것이다.
    // 텍스처 레지스터에 텍스처가 정상적으로 바인딩이 되었는지의 여부를 재질 상수 버퍼로 전달하여,
    // 그 값에 따라 샘플링 / 마젠타 출력 여부를 결정하게 한다
    
    if (g_btex_0)        // 텍스처 바인딩이 제대로 되었다면(bool값이 1이라면) 텍스처 출력
    {
        vOutColor = g_tex_0.Sample(g_sam_1, _in.vUV);
    }
    else // 텍스처 바인딩이 되어있지 않다면(bool값이 0이라면) 아무것도 출력하지 않음
    {
        vOutColor = float4(0.f, 0.f, 0.f, 0.f);
    }
    
    // 알파값이 0인 경우에는 버린다.
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
// g_int_0              : 줄어드는거 사용 여부(TimerMtrl만 사용)
// g_float_0            : TimePercentage

// g_tex_0              : Output Texture
// ========================================

VS_OUT VS_Std2D_UI(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f; // HLSL 구조체 초기화
    
   // 동차좌표를 1로 확장한(좌표) 로컬 Pos에 월드*뷰*투영을 곱해놓은 matWVP 한번에 곱하여 최종 투영행렬 반환
    output.vPosition = mul(float4(_in.vLocalPos, 1.f), g_matWVP);
    output.vUV = _in.vUV; // UV값은 그대로 전달
    
    return output;
}

float3 PS_Std2D_UI(VS_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f; // 초기화
    
    if (g_btex_0)        // 텍스처 바인딩이 제대로 되었다면(bool값이 1이라면) 텍스처 출력
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
    else // 텍스처 바인딩이 되어있지 않다면(bool값이 0이라면) 아무것도 출력하지 않음
    {
        vOutColor = float4(0.f, 0.f, 0.f, 0.f);
    }
    
    // 알파값이 0인 경우에는 버린다.
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
// g_int_0              : AnimUse (애니메이션 사용 여부)
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
       
    if (g_btex_0)       // 텍스처 바인딩이 된 경우
    {
        // 애니메이션 사용시
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
        // 애니메이션 사용하지 않을 시
        else
        {
            // 바인딩된 텍스처 렌더링
            vOutColor = g_tex_0.Sample(g_sam_1, _in.vUV);
        }
    }
    else // 바인딩된 텍스처가 없을 경우 출력하지 않음
    {
        vOutColor = float4(0.f, 0.f, 0.f, 0.f);
    }
    
    float3 vNormal = (float3) 0.f;
    
    // 노말 전용 텍스쳐가 있는 경우 노멀값을 계산하여 반영한다.
    if (g_btex_1)
    {
        // Normal 값 추출  (샘플링을 타고 오면 0~1 범위가 된다)
        vNormal = g_tex_1.Sample(g_sam_1, _in.vUV);
        
        // 0 ~ 1 범위를 -1 ~ 1 로 변경 (-1 : 빛의 방향과 정반대, 1 : 빛을 마주보는 방향)
        vNormal = (vNormal * 2.f) - 1.f;
        
        // NormalTexture 좌표계는 y축과 z 축이 반대로 되어있다.
        float f = vNormal.y;
        vNormal.y = vNormal.z;
        vNormal.z = f;
        
        // Texture 에서 추출한 Normal 방향을 월드로 변환시킨다. (회전파트만 적용시켜야 하므로, 동차좌표는 0, 노멀라이즈해준다.)
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
    
    // Lighting 처리
    tLightColor LightColor = (tLightColor) 0.f;
    
    // vNormal 이 Zero Vector 라면 (들어온 노멀값이 없다면)
    if (dot(vNormal, vNormal) == 0.f)
    {
        CalcLight2D(_in.vWorldPos, LightColor);
    }
    else
    {
        CalcLight2D(_in.vWorldPos, vNormal, LightColor);
    }
    
    // 반복문을 돌며 구해온 최종 vLightColor 값을 출력하고자 하는 픽셀 색에 곱해준다.
    vOutColor.rgb *= (LightColor.vDiffuse.rgb + LightColor.vAmbient.rgb);
    
    return vOutColor;
}


VS_OUT VS_Std2D_Blood(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f; // HLSL 구조체 초기화
    
   // 동차좌표를 1로 확장한(좌표) 로컬 Pos에 월드*뷰*투영을 곱해놓은 matWVP 한번에 곱하여 최종 투영행렬 반환
    output.vPosition = mul(float4(_in.vLocalPos, 1.f), g_matWVP);
    output.vUV = _in.vUV; // UV값은 그대로 전달
    
    return output;
}

float3 PS_Std2D_Blood(VS_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f; // 초기화
       
    if (g_btex_0)       // 텍스처 바인딩이 된 경우
    {
        // 애니메이션 사용시
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
        // 애니메이션 사용하지 않을 시
        else
        {
            // 바인딩된 텍스처 렌더링
            vOutColor = g_tex_0.Sample(g_sam_1, _in.vUV);
        }
    }
    else // 바인딩된 텍스처가 없을 경우 출력하지 않음
    {
        vOutColor = float4(0.f, 0.f, 0.f, 0.f);
    }
    
    // 알파값이 0인 경우에는 버린다.
    if (0.05f >= vOutColor.a)
        discard;
    
    float4 _TintColor = float4(0.8f, 0.f, 0.f, 1.f);
    
    vOutColor.rgb *= _TintColor.rgb;
    
    return vOutColor;
}





#endif
