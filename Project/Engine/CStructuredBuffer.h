#pragma once
#include "CEntity.h"

// 텍스트 레지스터를 이용하여 가변적인 크기로 바인딩할 수 있는 구조화 버퍼
// 누군가 관리하는 것이 아니라 필요시 그때그때 만들어서 사용하는 식이다.


// CPU 데이터(SysMem),  Gpu 데이터, GPU 레지스터(셰이더 단계)
// 하나의 구조화버퍼가 이 3가지 구역에서 자유롭게 데이터를 주고받을 수 없다.
// SysMem <-> Gpu, Gpu <-> Gpu 레지스터(셰이더 단계) 이렇게 따로따로 가능.

// m_bSysAccess : CpuAccess (CPU(SysMem)와 GPU 간의 데이터 전송가능 여부) true / false
// SB_TYPE : (Gpu와 셰이더 단계간의 데이터 전송가능 여부) ReadOnly / Read/Write 
// 즉, 4가지 조합의 구조화 버퍼가 생성될 수 있다.

// CpuAccess : false, SB_TYPE:: ReadOnly : 레지스터가 읽기만 가능(텍스처 등을 출력만 하는 경우 ex.Light: t 레지스터 바인딩)
// CpuAccess : false, SB_TYPE:: ReadWrite : 레지스터 읽기 쓰기 가능(ex. Cpu 접근이 필요 없는 CS 사용하는 파티클 데이터 : u 레지스터 바인딩)
// CpuAccess : true,  SB_TYPE:: ReadOnly : SetData로 버퍼는 수정가능하나 CS는 필요 없음 (ex. 타일맵)
// CpuAccess : true,  SB_TYPE:: ReadWrite : 양쪽 버퍼 모두 수정 가능 (Cpu에서 초기값 주고, CS도 활용하는 식의 경우)


class CStructuredBuffer :
    public CEntity
{
public:
    CStructuredBuffer();
    ~CStructuredBuffer();

public:
    CLONE_DEACTIVATE(CStructuredBuffer);
    
private:
    ComPtr<ID3D11Buffer>                m_SB;
    ComPtr<ID3D11ShaderResourceView>    m_SRV;
    ComPtr<ID3D11UnorderedAccessView>   m_UAV;

    D3D11_BUFFER_DESC                   m_tDesc;

    // 시스템 메모리도 읽기/쓰기가 동시에 가능한 버퍼는 만들수 없으므로, 읽기용 / 쓰기용 버퍼 2개 추가 생성
    ComPtr<ID3D11Buffer>                m_SB_CPU_Read;  // GPU -> Sys
    ComPtr<ID3D11Buffer>                m_SB_CPU_Write; // Sys -> GPU

    UINT                                m_iElementSize;
    UINT                                m_iElementCount;

    SB_TYPE                             m_Type;
    bool                                m_bSysAccess;   // GPU <-> Sys(CPU) 간의 접근가능 여부

    UINT                                m_iRecentRegisterNum;   // 가장 최근 바인딩한 레지스터 번호


public:
    // SB_TYPE : GPU 레지스터(셰이더) 접근 여부
    // _bUseSysAccess : 시스템메모리(CPU) 접근 여부
    //_SysMem : 초기값 세팅. 디폴트 인자로, 입력하지 않으면 nullptr로 들어감
    void Create(UINT _iElementSize, UINT _iElementCount, SB_TYPE _Type, bool _bUseSysAccess, void* _pSysMem = nullptr);
    void SetData(void* _pSrc, UINT _iSize = 0);
    void GetData(void* _pDst);

    // PIPELINE_STAGE
    void UpdateData(UINT _iRegisterNum, UINT _iPipeLineStage);

    // ComputeShader 바인딩 -> _IsShaderRes : 셰이더리소스뷰에 바인딩할지(읽기전용) 여부
    void UpdateData_CS(UINT _iRegisterNum, bool _IsShaderRes); 

    void Clear();
    void Clear_CS(bool _IsShaderRes); // _IsShaderRes : 셰이더리소스뷰에 바인딩한건지(읽기전용) 여부

public:
    UINT GetElementSize() { return m_iElementSize; }
    UINT GetElementCount() { return m_iElementCount; }
    UINT GetBufferSize() { return m_iElementSize * m_iElementCount; }

};

