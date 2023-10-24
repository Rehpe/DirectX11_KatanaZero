#ifndef _PARTICLE_RENDER
#define _PARTICLE_RENDER

#include "value.fx"
#include "struct.fx"


// ========================
// Particle Render Shader
// mesh : PointMesh

// Parameter
// g_int_0 : Particle Index

// g_int_1              : AnimUse (�ִϸ��̼� ��� ����)
// g_vec2_0             : AnimAtlas LeftTop
// g_vec2_1             : AnimAtlas Slice
// g_vec2_2             : AnimAtlas Offset
// g_vec2_3             : AnimAtlas BackSize

#define bAnimUse        g_int_1
#define LeftTop         g_vec2_0
#define Slice           g_vec2_1
#define Offset          g_vec2_2
#define BackSize        g_vec2_3

// =========================

// ��ƼŬ ������ ���̴��̹Ƿ�, t�������Ϳ� ���ε��� ����ȭ ���� ���
StructuredBuffer<tParticle>         ParticleBuffer : register(t20);
StructuredBuffer<tParticleModule>   ParticleModuleData : register(t21);
#define ModuleData                  ParticleModuleData[0]


struct VS_IN
{
    float3 vPos : POSITION;
    uint iInstID : SV_InstanceID;   //  DrawIndexedInstanced �Լ� ������, ���� �ݺ����� �ε��� i : ������ ���� ������ �ݺ�Ƚ��
                                    //  �� ���� ParticleBuffer�� �ε����� Ȱ���ϸ�, ��� ������ �ε����� ������ �������� �����Ͽ�
                                    //  ������ ���� �� ������������ �� �� ���� ������ ��ó�� ���� ���̴�(Drawcall 1ȸ) = �ν��Ͻ�
};

struct VS_OUT
{
    float3 vPos : POSITION;         // SV_Position : �����Ͷ��������� �������� �ѱ�� ���� ���̴� �ø�ƽ����, GS�ܰ谡 �������� VS�ܰ迡�� ����� �� ����.
    uint iInstID : SV_InstanceID;
};

VS_OUT VS_ParticleRender(VS_IN _in)
{
    // GS�ܰ谡 �������Ƿ�, ���� ����, ��ƼŬ�� ��ǥ ��� ���� GS�ܰ迡�� �ϰ� �ȴ�.
    // ����, VS�� ���� �����͸� �״�� �޾� output���� ���� �ܰ�� �Ѱ��ִ� ���Ҹ� �Ѵ�.
    
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPos = _in.vPos;
    output.iInstID = _in.iInstID;
    
    return output;  
}

// GeometryShader ���
// 1. ���������� ���� (GS�ܰ迡�� ���� ������ ���� �ʴ´ٸ� �״�� ���������� ���� : ��ƼŬ�� Ȱ��/��Ȱ��) 
// 2. ������ ó�� (��𿡼� �Ĵٺ��� �� ī�޶� ���ֺ���) -> View�����̽�����.

struct GS_OUT
{
    float4 vPosition : SV_Position;     // �����Ͷ����� �ܰ迡 �ѱ� ���� ��ġ����
    float2 vUV : TEXCOORD;
    uint iInstID : SV_InstanceID;
};

[maxvertexcount(6)] // GS �ܰ迡�� ���� 6�� ����ڴٴ� ��(�ﰢ�� �ΰ��� �̷���� �簢�� ����� ����� ����)
void GS_ParticleRender(point VS_OUT _in[1], inout TriangleStream<GS_OUT> _outstream)
{
    // ������ point�� �ǹ�: �� �ϳ��� ȣ���� �Լ���� ��
    // TriangleStream : �� 3���� �����ؼ� ���� ���� ���̶�� ��
    // TriangeStream�� ��ġ ���Ͱ��� ��(GS_OUT Ÿ���� ���� ���Ͷ�� �����ϸ� �ȴ�)
    // append�Լ��� �� ���͸� ä���� ����� �ȴ�.
    
    uint id = _in[0].iInstID;
    
    // ===== ��Ȱ��ȭ ��======
    // �ƹ��͵� ���� �ʰ� return -> ���������� ����
    if (0 == ParticleBuffer[id].Active)
        return;

    // ===== Ȱ��ȭ �� ======
     
    // 1. ������ ó���� ���� �ϴ� ���� ����Ʈ�� ������� ���� �佺���̽��� ������.(��ǥ���̴� 1�� Ȯ��)
    float3 vParticleViewPos = mul(float4(ParticleBuffer[id].vWorldPos.xyz, 1.f), g_matView).xyz;
    
    // 2. �ش� ��ƼŬ�� ũ�Ⱚ�� �˾ƿ´�. ������ �簢������ Ȯ���� �� �ش� ũ��� Ű��� ���� ���ȴ�.
    float2 vParticleScale = ParticleBuffer[id].vWorldScale.xy * ParticleBuffer[id].ScaleFactor;
    
    // 3. ���� ����
    
    // 0 -- 1
    // |    |
    // 3 -- 2
    float3 NewPos[4] =  // �������� Scale��ŭ�� ũ��� ���� 4���� ��´�.
    {
        float3(-vParticleScale.x / 2.f,  vParticleScale.y / 2.f, 0.f),
        float3( vParticleScale.x / 2.f,  vParticleScale.y / 2.f, 0.f),
        float3( vParticleScale.x / 2.f, -vParticleScale.y / 2.f, 0.f),
        float3(-vParticleScale.x / 2.f, -vParticleScale.y / 2.f, 0.f)
    };
    
    //���� ��� ����
    if (ModuleData.Render)
    {
        // ���� ����� �ӵ� ��� ũ�� ��� ��
        if (ModuleData.VelocityScale)
        {
            // ���� ��ƼŬ�� �ӷ��� �˾Ƴ���. (���� �ӵ��� ������ �ְ� �ӵ����� ũ�ٸ�, �ְ� �ӵ��� ����)
            float fCurSpeed = length(ParticleBuffer[id].vVelocity);
            if (ModuleData.vMaxSpeed < fCurSpeed)
                fCurSpeed = ModuleData.vMaxSpeed;
            
            // �ִ�ӵ� ��� ���� �ӵ��� ������ ���Ѵ�.
            float fRatio = saturate(fCurSpeed / ModuleData.vMaxSpeed);
          
            // ������ �´� ũ�⺯ȭ���� ���Ѵ�.
            float3 vDefaultScale = float3(1.f, 1.f, 1.f);
            float3 fScale = vDefaultScale + (ModuleData.vMaxVelocityScale.xyz - vDefaultScale) * fRatio;
            
            // ��ƼŬ�� �߾��� �ƴ�, �Ӹ��� �������� ������ �پ��� ȿ���� �ֱ� ���� �����κп��� ����
            NewPos[0] = NewPos[0] * fScale;
            NewPos[3] = NewPos[3] * fScale;
        }
        
        // ���� ����� �ӵ����� ��� ��
        if (ModuleData.VelocityAlignment)
        {
            // ��ƼŬ ���� ���� �ӵ��� View �������� ��ȯ
            float3 vVelocity = normalize(ParticleBuffer[id].vVelocity);
            vVelocity = mul(float4(vVelocity, 0.f), g_matView).xyz;
                       
            // ��ƼŬ�� Right ����� �̵� ������ �����Ͽ� �� ������ ������ ���Ѵ�.
            float3 vRight = float3(1.f, 0.f, 0.f);
            float fTheta = acos(dot(vRight, vVelocity));
            
            if (vVelocity.y < vRight.y) // �̵� ������ y���� r������ y��(���������̱⿡ �� 0�̴�)���� �۴ٸ�
            {
                // ������ ����� �׻� ����(�� ���� ����)�� �������� �ϱ� ������, 
                // �� ū ������ ���ϴ� ��� (Velocity�� y���� ������ ���) 
                // 2����(360��)���� ��Ÿ�� ���־� ���� ������ ���Ѵ�.
                fTheta = (2.f * 3.1415926535f) - fTheta;
            }
            
            // ���� ������ Z �� ȸ������� �����.
            float3x3 matRotZ =
            {
                 cos(fTheta), sin(fTheta),    0,
                -sin(fTheta), cos(fTheta),    0,
                           0,           0,  1.f,
            };
            
            // �� ������ ȸ������� �����ش�.
            for (int i = 0; i < 4; ++i)
            {
                NewPos[i] = mul(NewPos[i], matRotZ);
            }
        }
        
        if (ModuleData.VelocityScale)
        {
            
        }
    }
    
   
    
    // ����� GS_OUT Ÿ���� output �ʱ�ȭ (��ƼŬ �ϳ��� ������ 4���̹Ƿ�)
    GS_OUT output[4] = { (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f };
    
    // �� �������� ��������� ���� ���� ��ǥ(������ǥ)�� �����, UV���� �������ش�.
    output[0].vPosition = mul(float4(NewPos[0] + vParticleViewPos, 1.f), g_matProj);
    output[0].vUV = float2(0.f, 0.f);
    output[0].iInstID = id;
    
    output[1].vPosition = mul(float4(NewPos[1] + vParticleViewPos, 1.f), g_matProj);
    output[1].vUV = float2(1.f, 0.f);
    output[1].iInstID = id;
    
    output[2].vPosition = mul(float4(NewPos[2] + vParticleViewPos, 1.f), g_matProj);
    output[2].vUV = float2(1.f, 1.f);
    output[2].iInstID = id;
    
    output[3].vPosition = mul(float4(NewPos[3] + vParticleViewPos, 1.f), g_matProj);
    output[3].vUV = float2(0.f, 1.f);
    output[3].iInstID = id;
    
    // ���� ������ ������ �������� ���������� ������ �����Ѵ�.
    _outstream.Append(output[0]);
    _outstream.Append(output[1]);
    _outstream.Append(output[2]);
    _outstream.RestartStrip();      // �̷��԰� �ﰢ���� �̷�ٴ� ��(���� 3������ ȣ��)
    
    _outstream.Append(output[0]);
    _outstream.Append(output[2]);
    _outstream.Append(output[3]);
    _outstream.RestartStrip();      // �̷��԰� �ﰢ���� �̷�ٴ� ��(���� 3������ ȣ��)
}

float4 PS_ParticleRender(GS_OUT _in) : SV_Target
{
    float4 vOutColor = float4(1.f, 0.f, 1.f, 1.f);
    
    // ���ε��� �ؽ�ó�� ���� ���
    if (g_btex_0)
    {
         // �ִϸ��̼� ����
        if (bAnimUse)
        {
            float2 vUV = LeftTop + (BackSize * _in.vUV);
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
                //vOutColor = float4(1.f, 1.f, 0.f, 1.f);
                discard;
            }
        }
        else
        {
            vOutColor = g_tex_0.Sample(g_sam_0, _in.vUV);
            vOutColor.rgb *= ParticleBuffer[_in.iInstID].vColor.rgb;
        }

    }
    
    return vOutColor;
}


#endif
