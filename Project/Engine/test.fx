#ifndef _TEST
#define _TEST

// 전역 레지스터 변수만 모아놓은 value.fx 인클루드
#include "value.fx"

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

#define bAnimUse        g_int_0
#define LeftTop         g_vec2_0
#define Slice           g_vec2_1
#define Offset          g_vec2_2
#define BackSize        g_vec2_3

// g_tex_0              : Output Texture
// g_tex_1              : Nomal Texture
// ======================================

// VS(Vertex Shader) 입력 구조체
struct VS_IN
{
	float3 vPos     : POSITION;		//semantic
    float4 vColor   : COLOR;
    float2 vUV      : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float4 vOutColor : COLOR;
    float2 vOutUV    : TEXCOORD;
};

// Vertex Shader 함수
// LocalSpace 물체를 NDC 좌표계로 이동
VS_OUT VS_Test(VS_IN _in)
{
   // VS_OUT output = (VS_OUT) 0.f;       //HLSL의 구조체 초기화 문법

   // // 입력으로 들어온 Vec3 타입의 좌표를 동차행렬로 마지막 값을 1.f로 채워서 받아온 후 World행렬을 곱해준다.
   //float4 vWorldPos = mul(float4(_in.vPos, 1.f), g_matWorld);
   //float4 vViewPos = mul(vWorldPos, g_matView);
   //float4 vProjPos = mul(vViewPos, g_matProj);
    
   // output.vPosition = vProjPos;
   // output.vOutColor = _in.vColor;
   // output.vOutUV = _in.vUV;
    
   // return output;   
    
    VS_OUT output = (VS_OUT) 0.f;
    output.vPosition = float4(_in.vPos.xy * 2.f, 0.5f, 1.f);
    output.vOutUV = _in.vUV;
    
    return output;

}

// ===== 반환 구조체 사용하지 않을 경우 Pixel Shader 함수 ======
float4 PS_Test(VS_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
       
    if (g_btex_0)       // 텍스처 바인딩이 된 경우
    {
        // 애니메이션 사용시
        if (bAnimUse)
        {
            float2 vUV = LeftTop + (BackSize * _in.vOutUV);
            vUV -= ((BackSize - Slice) / 2.f);
            vUV -= Offset;
            
            if (LeftTop.x < vUV.x && vUV.x < LeftTop.x + Slice.x
                && LeftTop.y < vUV.y && vUV.y < LeftTop.y + Slice.y)
            {
                vOutColor = g_tex_0.Sample(g_sam_0, vUV);
            }
            else
            {
                // UV를 초과한 바깥 부분은 버린다.
               // vOutColor = float4(1.f, 1.f, 0.f, 1.f);
                discard;
            }
        }
        // 애니메이션 사용하지 않을 시
        else
        {
            // 바인딩된 텍스처 렌더링
            vOutColor = g_tex_0.Sample(g_sam_0, _in.vOutUV);
        }
    }
    else // 바인딩된 텍스처가 없을 경우 마젠타색 출력
    {
        vOutColor = float4(1.f, 0.f, 1.f, 1.f);
    }
    
    return vOutColor;
}

#endif