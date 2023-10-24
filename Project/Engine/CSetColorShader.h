#pragma once
#include "CComputeShader.h"

#include "ptr.h"
#include "CTexture.h"

class CSetColorShader :
    public CComputeShader
{

public:
    // CSetColorShader의 경우 생성자에서 그룹 당 쓰레드 개수를 입력받는다.
    // 단, x,y,z의 곱이 1024(HLSL 5.0의 최대 지원 쓰레드 개수)를 넘으면 안된다.
    CSetColorShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY, UINT _iGroupPerThreadZ);
    ~CSetColorShader();

private:
    Ptr<CTexture>   m_OutTex;

public:
    void SetTargetTexture(Ptr<CTexture> _Tex) { m_OutTex = _Tex; }
    void SetColor(Vec3 _RGB) { m_Const.arrV4[0] = _RGB; }

public:
    virtual void UpdateData();
    virtual void Clear();

};