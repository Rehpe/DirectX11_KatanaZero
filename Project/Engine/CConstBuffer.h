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
    ComPtr<ID3D11Buffer>    m_CB;	//��� ����
    UINT                    m_iRegisterNum;
    D3D11_BUFFER_DESC       m_Desc;

    UINT                    m_iElementSize;
    UINT                    m_iElementCount;

public:
    void Create(UINT _iElementSize, UINT _iElementCount);   // ��� ���� ����
    void SetData(void* _pSrc, UINT _iSize = 0);             // ��� ������ Map/Unmap (������ �������� �ʾ��� �� m_iElementSize*m_iElementCount)
    void UpdateData();                                      // ����������(�׷��ȼ��̴�) ���ε�
    void UpdateData_CS();                                   // ��ǻ�ͼ��̴� ���ε�
};

