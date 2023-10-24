#pragma once
#include "CShader.h"

// 컴퓨트 셰이더의 경우 처리해야할 일에 따라 텍스처, 바인딩 등의 필요 여부가 제각각 다르다
// 따라서 컴퓨터 셰이더의 경우 가장 기본적인 멤버와 함수를 가진 부모 컴퓨트 셰이더를
// 상속받아 독립된 컴퓨터 셰이더로 사용될 것이다

class CComputeShader :
    public CShader
{
public:
    CComputeShader();
    virtual ~CComputeShader();

public:
    CLONE_DEACTIVATE(CComputeShader);

private:
    ComPtr<ID3D11ComputeShader>     m_CS;
    ComPtr<ID3DBlob>                m_CSBlob;

protected:
    // 쉐이더에 전달할 상수 데이터
    tMtrlConst                      m_Const;

    // 그룹 개수
    UINT                            m_iGroupX;
    UINT                            m_iGroupY;
    UINT                            m_iGroupZ;

    // 그룹 하나 당 스레드 개수
    UINT                            m_iGroupPerThreadX;
    UINT                            m_iGroupPerThreadY;
    UINT                            m_iGroupPerThreadZ;


public:
    void CreateComputeShader(const wstring& _strFileName, const string& _strFuncName);
    
    // 컴퓨터 셰이드 실행 함수. 
    void Execute(); // UpdateData / 상수버퍼 생성&바인딩 / Dispatch / Clear 과정이 들어있다
    
private:
    // 컴퓨터 셰이드를 상속받은 자식들은 무조건 UpdateData, Clear을 구현해야한다.
    virtual void UpdateData() = 0;
    virtual void Clear() = 0;

};

