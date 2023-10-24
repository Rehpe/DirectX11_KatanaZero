#ifndef _VALUE
#define _VALUE

#include "struct.fx"

// 전역 레지스터 변수들만 따로 모아둔 .fx 파일

// 상수 버퍼와 연결될 레지스터 메모리
// GPU는 행렬을 행*열이 아닌 열*행 순서로 읽기 때문에, row_major 키워드로 전치를 해줘서 GPU에게 행*열 순서로 읽게 명령해줘야한다
cbuffer TRANSFORM : register(b0)
{
    row_major matrix g_matWorld;    // World 행렬
    row_major matrix g_matView;     // View (Camera) Space 행렬
    row_major matrix g_matProj;     // 투영 행렬 (뷰 행렬을 최종 NDC 좌표계(-1 ~ 1)에 축소배치(투영)하여 찍을 정보)
    
    row_major matrix g_matWV;       // World 행렬 * View 행렬
    row_major matrix g_matWVP;      // World 행렬 * View 행렬 * 투영(Projection) 행렬
};

cbuffer MATERIAL : register(b1)
{
    int g_int_0;
    int g_int_1;
    int g_int_2;
    int g_int_3;
    
    float g_float_0;
    float g_float_1;
    float g_float_2;
    float g_float_3;
    
    float2 g_vec2_0;
    float2 g_vec2_1;
    float2 g_vec2_2;
    float2 g_vec2_3;
    
    float4 g_vec4_0;
    float4 g_vec4_1;
    float4 g_vec4_2;
    float4 g_vec4_3;
    
    matrix g_mat_0;
    matrix g_mat_1;
    matrix g_mat_2;
    matrix g_mat_3;
    
    int    g_btex_0;
    int    g_btex_1;
    int    g_btex_2;
    int    g_btex_3;
    int    g_btex_4;
    int    g_btex_5;
    int    g_btex_6;
    int    g_btex_7;
};

// 게임에서 전역으로 사용할 상수를 관리할 것이다.
cbuffer GLOBAL : register(b2)
{
    float2 g_Resolution;    // 해상도
    float g_DT;             // DeltaTime
    float g_AccTime;        // 누적 시간
    uint g_Light2DCount;    // 2D 광원 갯수
    uint g_Light3DCount;    // 3D 광원 갯수
    int2 g_globalpadding;   // 16바이트를 맞추기 위한 패딩
}


// 텍스처와 바인딩된 T0 레지스터는 읽기 전용 레지스터
Texture2D g_tex_0 : register(t0);
Texture2D g_tex_1 : register(t1);
Texture2D g_tex_2 : register(t2);
Texture2D g_tex_3 : register(t3);
Texture2D g_tex_4 : register(t4);
Texture2D g_tex_5 : register(t5);
Texture2D g_tex_6 : register(t6);
Texture2D g_tex_7 : register(t7);

// 큐브텍스처
TextureCube g_cube_0 : register(t8);
TextureCube g_cube_1 : register(t9);

// 배열텍스처
Texture2DArray g_arrtex_0 : register(t10);
Texture2DArray g_arrtex_1 : register(t11);


// 광원 정보로 사용하는 구조화 버퍼
StructuredBuffer<tLightInfo> g_Light2DBuffer : register(t12);
StructuredBuffer<tLightInfo> g_Light3DBuffer : register(t13);

// 샘플러
SamplerState g_sam_0 : register(s0); // 이방성 샘플링
SamplerState g_sam_1 : register(s1); // 필터 없는 가장 기본적인 샘플러



#endif
