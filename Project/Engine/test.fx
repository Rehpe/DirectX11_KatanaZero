#ifndef _TEST
#define _TEST

// ���� �������� ������ ��Ƴ��� value.fx ��Ŭ���
#include "value.fx"

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

#define bAnimUse        g_int_0
#define LeftTop         g_vec2_0
#define Slice           g_vec2_1
#define Offset          g_vec2_2
#define BackSize        g_vec2_3

// g_tex_0              : Output Texture
// g_tex_1              : Nomal Texture
// ======================================

// VS(Vertex Shader) �Է� ����ü
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

// Vertex Shader �Լ�
// LocalSpace ��ü�� NDC ��ǥ��� �̵�
VS_OUT VS_Test(VS_IN _in)
{
   // VS_OUT output = (VS_OUT) 0.f;       //HLSL�� ����ü �ʱ�ȭ ����

   // // �Է����� ���� Vec3 Ÿ���� ��ǥ�� ������ķ� ������ ���� 1.f�� ä���� �޾ƿ� �� World����� �����ش�.
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

// ===== ��ȯ ����ü ������� ���� ��� Pixel Shader �Լ� ======
float4 PS_Test(VS_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
       
    if (g_btex_0)       // �ؽ�ó ���ε��� �� ���
    {
        // �ִϸ��̼� ����
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
                // UV�� �ʰ��� �ٱ� �κ��� ������.
               // vOutColor = float4(1.f, 1.f, 0.f, 1.f);
                discard;
            }
        }
        // �ִϸ��̼� ������� ���� ��
        else
        {
            // ���ε��� �ؽ�ó ������
            vOutColor = g_tex_0.Sample(g_sam_0, _in.vOutUV);
        }
    }
    else // ���ε��� �ؽ�ó�� ���� ��� ����Ÿ�� ���
    {
        vOutColor = float4(1.f, 0.f, 1.f, 1.f);
    }
    
    return vOutColor;
}

#endif