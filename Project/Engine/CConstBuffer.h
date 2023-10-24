#pragma once
#include "CEntity.h"
class CConstBuffer :
    public CEntity
{
public:
    CConstBuffer(UINT _iRegisterNum);
    ~CConstBuffer();

public:
    CLONE_DEACTIVATE(CConstBuffer);

private:
    ComPtr<ID3D11Buffer>    m_CB;	//상수 버퍼
    UINT                    m_iRegisterNum;
    D3D11_BUFFER_DESC       m_Desc;

    UINT                    m_iElementSize;
    UINT                    m_iElementCount;

public:
    void Create(UINT _iElementSize, UINT _iElementCount);   // 상수 버퍼 생성
    void SetData(void* _pSrc, UINT _iSize = 0);             // 상수 버퍼의 Map/Unmap (사이즈 지정하지 않았을 시 m_iElementSize*m_iElementCount)
    void UpdateData();                                      // 파이프라인(그래픽셰이더) 바인딩
    void UpdateData_CS();                                   // 컴퓨터셰이더 바인딩
};

