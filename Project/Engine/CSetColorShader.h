#pragma once
#include "CComputeShader.h"

#include "ptr.h"
#include "CTexture.h"

class CSetColorShader :
    public CComputeShader
{

public:
    // CSetColorShader�� ��� �����ڿ��� �׷� �� ������ ������ �Է¹޴´�.
    // ��, x,y,z�� ���� 1024(HLSL 5.0�� �ִ� ���� ������ ����)�� ������ �ȵȴ�.
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