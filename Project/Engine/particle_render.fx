#ifndef _PARTICLE_RENDER
#define _PARTICLE_RENDER

#include "value.fx"
#include "struct.fx"


// ========================
// Particle Render Shader
// mesh : PointMesh

// Parameter
// g_int_0 : Particle Index

// g_int_1              : AnimUse (애니메이션 사용 여부)
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

// 파티클 렌더용 셰이더이므로, t레지스터에 바인딩된 구조화 버퍼 사용
StructuredBuffer<tParticle>         ParticleBuffer : register(t20);
StructuredBuffer<tParticleModule>   ParticleModuleData : register(t21);
#define ModuleData                  ParticleModuleData[0]


struct VS_IN
{
    float3 vPos : POSITION;
    uint iInstID : SV_InstanceID;   //  DrawIndexedInstanced 함수 내에서, 내부 반복문의 인덱스 i : 렌더링 과정 내부의 반복횟수
                                    //  이 값을 ParticleBuffer의 인덱스로 활용하면, 모든 버퍼의 인덱싱을 끝내고 렌더링을 종료하여
                                    //  겉으로 봤을 때 파이프라인을 딱 한 번만 진행한 것처럼 보일 것이다(Drawcall 1회) = 인스턴싱
};

struct VS_OUT
{
    float3 vPos : POSITION;         // SV_Position : 래스터라이저에게 최종으로 넘기는 값에 붙이는 시맨틱으로, GS단계가 생겼으니 VS단계에선 사용할 수 없다.
    uint iInstID : SV_InstanceID;
};

VS_OUT VS_ParticleRender(VS_IN _in)
{
    // GS단계가 생겼으므로, 정점 생성, 파티클의 좌표 계산 등은 GS단계에서 하게 된다.
    // 따라서, VS는 정점 데이터를 그대로 받아 output으로 다음 단계로 넘겨주는 역할만 한다.
    
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPos = _in.vPos;
    output.iInstID = _in.iInstID;
    
    return output;  
}

// GeometryShader 사용
// 1. 파이프라인 제어 (GS단계에서 정점 생성을 하지 않는다면 그대로 파이프라인 종료 : 파티클의 활성/비활성) 
// 2. 빌보드 처리 (어디에서 쳐다봐도 늘 카메라를 마주보는) -> View스페이스에서.

struct GS_OUT
{
    float4 vPosition : SV_Position;     // 래스터라이저 단계에 넘길 최종 위치정보
    float2 vUV : TEXCOORD;
    uint iInstID : SV_InstanceID;
};

[maxvertexcount(6)] // GS 단계에서 정점 6개 만들겠다는 뜻(삼각형 두개로 이루어진 사각형 모양을 만들기 위해)
void GS_ParticleRender(point VS_OUT _in[1], inout TriangleStream<GS_OUT> _outstream)
{
    // 인자의 point의 의미: 점 하나에 호출할 함수라는 뜻
    // TriangleStream : 점 3개를 구성해서 면을 만들 것이라는 뜻
    // TriangeStream은 마치 벡터같은 것(GS_OUT 타입을 담은 벡터라고 생각하면 된다)
    // append함수로 이 벡터를 채워야 출력이 된다.
    
    uint id = _in[0].iInstID;
    
    // ===== 비활성화 시======
    // 아무것도 하지 않고 return -> 파이프라인 종료
    if (0 == ParticleBuffer[id].Active)
        return;

    // ===== 활성화 시 ======
     
    // 1. 빌보드 처리를 위해 일단 들어온 포인트에 뷰행렬을 곱해 뷰스페이스로 보낸다.(좌표값이니 1로 확장)
    float3 vParticleViewPos = mul(float4(ParticleBuffer[id].vWorldPos.xyz, 1.f), g_matView).xyz;
    
    // 2. 해당 파티클의 크기값을 알아온다. 정점을 사각형으로 확대할 때 해당 크기로 키우기 위해 사용된다.
    float2 vParticleScale = ParticleBuffer[id].vWorldScale.xy * ParticleBuffer[id].ScaleFactor;
    
    // 3. 정점 생성
    
    // 0 -- 1
    // |    |
    // 3 -- 2
    float3 NewPos[4] =  // 원점에서 Scale만큼의 크기로 정점 4개를 찍는다.
    {
        float3(-vParticleScale.x / 2.f,  vParticleScale.y / 2.f, 0.f),
        float3( vParticleScale.x / 2.f,  vParticleScale.y / 2.f, 0.f),
        float3( vParticleScale.x / 2.f, -vParticleScale.y / 2.f, 0.f),
        float3(-vParticleScale.x / 2.f, -vParticleScale.y / 2.f, 0.f)
    };
    
    //렌더 모듈 사용시
    if (ModuleData.Render)
    {
        // 렌더 모듈의 속도 비례 크기 사용 시
        if (ModuleData.VelocityScale)
        {
            // 현재 파티클의 속력을 알아낸다. (현재 속도가 설정한 최고 속도보다 크다면, 최고 속도로 제한)
            float fCurSpeed = length(ParticleBuffer[id].vVelocity);
            if (ModuleData.vMaxSpeed < fCurSpeed)
                fCurSpeed = ModuleData.vMaxSpeed;
            
            // 최대속도 대비 현재 속도의 비율을 구한다.
            float fRatio = saturate(fCurSpeed / ModuleData.vMaxSpeed);
          
            // 비율에 맞는 크기변화량을 구한다.
            float3 vDefaultScale = float3(1.f, 1.f, 1.f);
            float3 fScale = vDefaultScale + (ModuleData.vMaxVelocityScale.xyz - vDefaultScale) * fRatio;
            
            // 파티클의 중앙이 아닌, 머리를 기준으로 꼬리가 줄어드는 효과를 주기 위해 꼬리부분에만 적용
            NewPos[0] = NewPos[0] * fScale;
            NewPos[3] = NewPos[3] * fScale;
        }
        
        // 렌더 모듈의 속도정렬 사용 시
        if (ModuleData.VelocityAlignment)
        {
            // 파티클 월드 기준 속도를 View 공간으로 변환
            float3 vVelocity = normalize(ParticleBuffer[id].vVelocity);
            vVelocity = mul(float4(vVelocity, 0.f), g_matView).xyz;
                       
            // 파티클의 Right 방향과 이동 방향을 내적하여 둘 사이의 각도를 구한다.
            float3 vRight = float3(1.f, 0.f, 0.f);
            float fTheta = acos(dot(vRight, vVelocity));
            
            if (vVelocity.y < vRight.y) // 이동 방향의 y값이 r벡터의 y값(단위벡터이기에 늘 0이다)보다 작다면
            {
                // 내적의 결과는 항상 예각(더 작은 각도)을 기준으로 하기 때문에, 
                // 더 큰 각도를 구하는 경우 (Velocity의 y값이 음수일 경우) 
                // 2파이(360도)에서 세타를 빼주어 작은 각도를 구한다.
                fTheta = (2.f * 3.1415926535f) - fTheta;
            }
            
            // 구한 각도로 Z 축 회전행렬을 만든다.
            float3x3 matRotZ =
            {
                 cos(fTheta), sin(fTheta),    0,
                -sin(fTheta), cos(fTheta),    0,
                           0,           0,  1.f,
            };
            
            // 각 정점에 회전행렬을 곱해준다.
            for (int i = 0; i < 4; ++i)
            {
                NewPos[i] = mul(NewPos[i], matRotZ);
            }
        }
        
        if (ModuleData.VelocityScale)
        {
            
        }
    }
    
   
    
    // 출력할 GS_OUT 타입의 output 초기화 (파티클 하나당 꼭지점 4개이므로)
    GS_OUT output[4] = { (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f };
    
    // 각 꼭지점에 투영행렬을 곱해 최종 좌표(투영좌표)를 만들고, UV값을 지정해준다.
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
    
    // 위의 꼭지점 정보를 바탕으로 최종적으로 정점을 생성한다.
    _outstream.Append(output[0]);
    _outstream.Append(output[1]);
    _outstream.Append(output[2]);
    _outstream.RestartStrip();      // 이렇게가 삼각형을 이룬다는 뜻(정점 3개마다 호출)
    
    _outstream.Append(output[0]);
    _outstream.Append(output[2]);
    _outstream.Append(output[3]);
    _outstream.RestartStrip();      // 이렇게가 삼각형을 이룬다는 뜻(정점 3개마다 호출)
}

float4 PS_ParticleRender(GS_OUT _in) : SV_Target
{
    float4 vOutColor = float4(1.f, 0.f, 1.f, 1.f);
    
    // 바인딩된 텍스처가 있을 경우
    if (g_btex_0)
    {
         // 애니메이션 사용시
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
                // UV를 초과한 바깥 부분은 버린다.
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
