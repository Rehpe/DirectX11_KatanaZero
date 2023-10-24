
#ifndef _SETCOLOR
#define _SETCOLOR

#include "value.fx"

RWTexture2D<float4> g_Output : register(u0);


// ComputeShader 입력 Semantic
// SV_GroupThreadID      : 그룹 내에서의 인덱스(3차원)
// SV_GroupID            : 그룹 인덱스
// SV_GroupIndex         : 그룹 내에서의 인덱스(1차원)
// SV_DispatchThreadID   : 전체 그룹 기준, 스레드의 인덱스(3차원)

// 스레드 그룹 개수(HLSL 5.0 기준 1024 제한)
[numthreads(32, 32, 1)]
void CS_SetColor(int3 _ID : SV_DispatchThreadID)
{
    if (380 <= _ID.y && _ID.y <= 388 || 636 <= _ID.x && _ID.x <= 644)
    {
        g_Output[_ID.xy] = float4(1.f, 1.f, 0.f, 1.f);
    }
}



#endif