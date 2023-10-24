#pragma once
#include "CShader.h"

// ��ǻƮ ���̴��� ��� ó���ؾ��� �Ͽ� ���� �ؽ�ó, ���ε� ���� �ʿ� ���ΰ� ������ �ٸ���
// ���� ��ǻ�� ���̴��� ��� ���� �⺻���� ����� �Լ��� ���� �θ� ��ǻƮ ���̴���
// ��ӹ޾� ������ ��ǻ�� ���̴��� ���� ���̴�

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
    // ���̴��� ������ ��� ������
    tMtrlConst                      m_Const;

    // �׷� ����
    UINT                            m_iGroupX;
    UINT                            m_iGroupY;
    UINT                            m_iGroupZ;

    // �׷� �ϳ� �� ������ ����
    UINT                            m_iGroupPerThreadX;
    UINT                            m_iGroupPerThreadY;
    UINT                            m_iGroupPerThreadZ;


public:
    void CreateComputeShader(const wstring& _strFileName, const string& _strFuncName);
    
    // ��ǻ�� ���̵� ���� �Լ�. 
    void Execute(); // UpdateData / ������� ����&���ε� / Dispatch / Clear ������ ����ִ�
    
private:
    // ��ǻ�� ���̵带 ��ӹ��� �ڽĵ��� ������ UpdateData, Clear�� �����ؾ��Ѵ�.
    virtual void UpdateData() = 0;
    virtual void Clear() = 0;

};

