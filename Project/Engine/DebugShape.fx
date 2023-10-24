#ifndef _DEBUGSHAPE
#define _DEBUGSHAPE

#include "value.fx"


// =========================
// DebugShape Shader
// 
// Tolpology  : LineStrip 
// RS_TYPE    : CULL_NONE
// DS_TYPE    : NO_TEST_NO_WRITE (디버그용이므로 깊이 자체가 존재할 수 없음)
// BS_TYPE    : Defalut          (블렌딩 없이 강제 출력)
//
// g_vec4_0 : OutColor (색상 정보)
// =========================


struct VS_DEBUG_IN
{
    float3 vPos : POSITION;
};


struct VS_DEBUG_OUT
{
     float4 vPosition : SV_Position;
    
};


VS_DEBUG_OUT VS_DebugShape(VS_DEBUG_IN _in)
{
    VS_DEBUG_OUT output = (VS_DEBUG_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    
    return output;
}


float4 PS_DebugShape(VS_DEBUG_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
    
    vOutColor = g_vec4_0;
    
    return vOutColor;
}


#endif