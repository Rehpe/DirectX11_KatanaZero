#ifndef _POSTPROCESS
#define _POSTPROCESS

#include "value.fx"

struct VS_IN
{
    float3 vLocalPos : POSITION;        // RectMesh의 로컬 좌표
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

// ============================
// GrayShader(흑백 셰이더)
// mesh     : RectMesh
// Domain   : DOMAIN_POSTPROCESS
// g_tex_0  : RederTarget Copy Texture
// ============================

VS_OUT VS_GrayShader(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;

    // 단순히 화면 전체를 복사하면 그만이므로 월드 변환 등이 필요 없다.
    // RectMesh의 기본 값은 -0.5~0.5 이므로, 2를 곱해주어 -1~1, 즉 꽉 찬 UV좌표를 만들어준다
    
    output.vPosition = float4(_in.vLocalPos * 2.f, 1.f);    
    output.vUV = _in.vUV;

    return output;
}

float4 PS_GrayShader(VS_OUT _in) : SV_Target
{
    // tex_0 자리에는 복사된 RenderTarget 텍스처가 바인딩될 것이다.
    float4 vColor = g_tex_0.Sample(g_sam_0, _in.vUV);

    // 색상의 RGB값을 모두 더해 3으로 나누면 회색조가 된다.
    float vAver = (vColor.r + vColor.g + vColor.b) / 3.f;

    vColor = float4(vAver, vAver, vAver, 1.f);

    return vColor;
}


// ============================
// Distortion Shader
// mesh : RectMesh
// Domain : DOMAIN_POSTPROCESS
// g_tex_0 : RederTarget Copy Texture
// g_tex_1 : Noise Texture
// ============================
VS_OUT VS_Distortion(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;

    output.vPosition = mul(float4(_in.vLocalPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;

    return output;
}

//float4 PS_Distortion(VS_OUT _in) : SV_Target
//{
//	float2 vUV = _in.vPosition.xy / g_Resolution;
//		
//	
//	float fChange = cos(( (vUV.x - g_AccTime * 0.05f) / 0.15f) * 2 * 3.1415926535f) * 0.05f;
//
//	vUV.y += fChange;
//
//	float4 vColor = g_tex_0.Sample(g_sam_0, vUV);
//	//vColor.r *= 2.f;
//
//	return vColor;
//}

float4 PS_Distortion(VS_OUT _in) : SV_Target
{
    float2 vUV = _in.vPosition.xy / g_Resolution;
	
	// Noise Texture 가 세팅이 되어 있다면
    if (g_btex_1)
    {
        float2 vNoiseUV = float2(_in.vUV.x - (g_AccTime * 0.2f), _in.vUV.y);
        float4 vNoise = g_tex_1.Sample(g_sam_0, vNoiseUV);

        vNoise = (vNoise - 0.5f) * 0.02f;

        vUV += vNoise.r;
    }

    float4 vColor = g_tex_0.Sample(g_sam_0, vUV);

    return vColor;
}

// Glitch Shader
float mod289(float x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

float3 permute(float3 x)
{
    return mod289(((x * 34.0) + 1.0) * x);
}

float snoise(float2 v)
{
    const float4 C = float4(0.211324865405187, // (3.0-sqrt(3.0))/6.0
                            0.366025403784439, // 0.5*(sqrt(3.0)-1.0)
                            -0.577350269189626, // -1.0 + 2.0 * C.x
                            0.024390243902439); // 1.0 / 41.0
    // First corner
    float2 i = floor(v + dot(v, C.yy));
    float2 x0 = v - i + dot(i, C.xx);

    // Other corners
    float2 i1;
    i1 = (x0.x > x0.y) ? float2(1.0, 0.0) : float2(0.0, 1.0);
    float4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;

    // Permutations
    i = mod289(i);
    float3 p = permute(permute(i.y + float3(0.0, i1.y, 1.0))
        + i.x + float3(0.0, i1.x, 1.0));

    float3 m = max(0.5 - float3(dot(x0, x0), dot(x12.xy, x12.xy), dot(x12.zw, x12.zw)), 0.0);
    m = m * m;
    m = m * m;

    float3 x = 2.0 * frac(p * C.www) - 1.0;
    float3 h = abs(x) - 0.5;
    float3 ox = floor(x + 0.5);
    float3 a0 = x - ox;

    m *= 1.79284291400159 - 0.85373472095314 * (a0 * a0 + h * h);

    float3 g;
    g.x = a0.x * x0.x + h.x * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}

float rand(float2 co)
{
    return frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453);
}


VS_OUT VS_Glitch(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;

    output.vPosition = float4(_in.vLocalPos * 2.f, 1.f);
    output.vUV = _in.vUV;

    return output;
}

float4 PS_Glitch(VS_OUT _in) : SV_Target
{
    float2 uv = _in.vUV;
    float time = g_AccTime * 2.0;

    // Create large, incidental noise waves
    float noise = max(0.0, snoise(float2(time, uv.y * 0.3)) - 0.3) * (1.0 / 0.7);

    // Offset by smaller, constant noise waves
    noise = noise + (snoise(float2(time * 10.0, uv.y * 2.4)) - 0.5) * 0.15;

    // Apply the noise as x displacement for every line
    float xpos = uv.x - noise * noise * 0.25;
    float4 fragColor = g_tex_0.Sample(g_sam_0, float2(xpos, uv.y));

    // Mix in some random interference for lines
    float randValue = rand(float2(uv.y, time));
    float3 randomColor = float3(randValue, randValue, randValue);
    fragColor.rgb = lerp(fragColor.rgb, randomColor, float3(noise * 0.3, noise * 0.3, noise * 0.3));

    // Apply a line pattern every 4 pixels
    if (floor(fmod(_in.vPosition.y * 0.25, 2.0)) == 0.0)
    {
        fragColor.rgb *= 1.0 - (0.15 * noise);
    }

    // Shift green/blue channels (using the red channel)
    fragColor.g = lerp(fragColor.r, g_tex_0.Sample(g_sam_0, float2(xpos + noise * 0.05, uv.y)).g, 0.25);
    fragColor.b = lerp(fragColor.r, g_tex_0.Sample(g_sam_0, float2(xpos - noise * 0.05, uv.y)).b, 0.25);


    return fragColor;
}

// New Distortion Shader

VS_OUT VS_NewDistortion(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;

    output.vPosition = float4(_in.vLocalPos * 2.f, 1.f);
    output.vUV = _in.vUV;

    return output;
}

float4 PS_NewDistortion(VS_OUT _in) : SV_Target
{
    // Get the UV Coordinate of your texture or Screen Texture
    float2 uv = _in.vUV;
    
    float speed = 60.0f;
    float intensity = 80.0f;
    
    // Modify the X coordinate by the sin of y to oscillate back and forth
    uv.x += sin(uv.y * 10.0 + g_AccTime * speed) / intensity;

    // Get the pixel color at the index
    float4 color = g_tex_0.Sample(g_sam_0, uv);
    
    return color;
}







#endif