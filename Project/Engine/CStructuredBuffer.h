#pragma once
#include "CEntity.h"

// �ؽ�Ʈ �������͸� �̿��Ͽ� �������� ũ��� ���ε��� �� �ִ� ����ȭ ����
// ������ �����ϴ� ���� �ƴ϶� �ʿ�� �׶��׶� ���� ����ϴ� ���̴�.


// CPU ������(SysMem),  Gpu ������, GPU ��������(���̴� �ܰ�)
// �ϳ��� ����ȭ���۰� �� 3���� �������� �����Ӱ� �����͸� �ְ���� �� ����.
// SysMem <-> Gpu, Gpu <-> Gpu ��������(���̴� �ܰ�) �̷��� ���ε��� ����.

// m_bSysAccess : CpuAccess (CPU(SysMem)�� GPU ���� ������ ���۰��� ����) true / false
// SB_TYPE : (Gpu�� ���̴� �ܰ谣�� ������ ���۰��� ����) ReadOnly / Read/Write 
// ��, 4���� ������ ����ȭ ���۰� ������ �� �ִ�.

// CpuAccess : false, SB_TYPE:: ReadOnly : �������Ͱ� �б⸸ ����(�ؽ�ó ���� ��¸� �ϴ� ��� ex.Light: t �������� ���ε�)
// CpuAccess : false, SB_TYPE:: ReadWrite : �������� �б� ���� ����(ex. Cpu ������ �ʿ� ���� CS ����ϴ� ��ƼŬ ������ : u �������� ���ε�)
// CpuAccess : true,  SB_TYPE:: ReadOnly : SetData�� ���۴� ���������ϳ� CS�� �ʿ� ���� (ex. Ÿ�ϸ�)
// CpuAccess : true,  SB_TYPE:: ReadWrite : ���� ���� ��� ���� ���� (Cpu���� �ʱⰪ �ְ�, CS�� Ȱ���ϴ� ���� ���)


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

    // �ý��� �޸𸮵� �б�/���Ⱑ ���ÿ� ������ ���۴� ����� �����Ƿ�, �б�� / ����� ���� 2�� �߰� ����
    ComPtr<ID3D11Buffer>                m_SB_CPU_Read;  // GPU -> Sys
    ComPtr<ID3D11Buffer>                m_SB_CPU_Write; // Sys -> GPU

    UINT                                m_iElementSize;
    UINT                                m_iElementCount;

    SB_TYPE                             m_Type;
    bool                                m_bSysAccess;   // GPU <-> Sys(CPU) ���� ���ٰ��� ����

    UINT                                m_iRecentRegisterNum;   // ���� �ֱ� ���ε��� �������� ��ȣ


public:
    // SB_TYPE : GPU ��������(���̴�) ���� ����
    // _bUseSysAccess : �ý��۸޸�(CPU) ���� ����
    //_SysMem : �ʱⰪ ����. ����Ʈ ���ڷ�, �Է����� ������ nullptr�� ��
    void Create(UINT _iElementSize, UINT _iElementCount, SB_TYPE _Type, bool _bUseSysAccess, void* _pSysMem = nullptr);
    void SetData(void* _pSrc, UINT _iSize = 0);
    void GetData(void* _pDst);

    // PIPELINE_STAGE
    void UpdateData(UINT _iRegisterNum, UINT _iPipeLineStage);

    // ComputeShader ���ε� -> _IsShaderRes : ���̴����ҽ��信 ���ε�����(�б�����) ����
    void UpdateData_CS(UINT _iRegisterNum, bool _IsShaderRes); 

    void Clear();
    void Clear_CS(bool _IsShaderRes); // _IsShaderRes : ���̴����ҽ��信 ���ε��Ѱ���(�б�����) ����

public:
    UINT GetElementSize() { return m_iElementSize; }
    UINT GetElementCount() { return m_iElementCount; }
    UINT GetBufferSize() { return m_iElementSize * m_iElementCount; }

};

