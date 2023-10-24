#pragma once
#include "CResource.h"

//�������� ������ ���ҽ�ȭ���� �����ϴ� Ŭ����

class CMesh :
    public CResource
{
public:
    CMesh(bool _bEngine = false);
    ~CMesh();

private:
	ComPtr<ID3D11Buffer>		m_VB;			//���� ����
	D3D11_BUFFER_DESC			m_tVBDesc;
	UINT						m_VtxCount;

	ComPtr<ID3D11Buffer>		m_IB;			//�ε��� ����
	D3D11_BUFFER_DESC			m_tIBDesc;
	UINT						m_IdxCount;

	D3D11_PRIMITIVE_TOPOLOGY	m_Topology;

	void* m_pVtxSys;						//GPU�� ������ų ���� ���� �ý��� �޸�
	void* m_pIdxSys;						//GPU�� ������ų �ε��� ���� �ý��� �޸�


	// �ϴ� ���������Լ� Ǫ�� ������ ���̷� �����س��ڴ�.
public:
	virtual int Save(const wstring& _strRelativePath) { return S_OK; }
private:
	virtual int Load(const wstring& _strFilePath) { return S_OK; }
   
public:
	void Create(void* _VtxSysMem, UINT _iVtxCount, void* _IdxSysMem, UINT _IdxCount);
	void CreateLine(void* _VtxSysMem, UINT _iVtxCount, void* _IdxSysMem, UINT _IdxCount);


	void render();
	void render_particle(UINT _iParticleCount);


	// render�� render_particle�� ����
	// render: �ϳ��� ����������(IA->VS->...OM) �� ��� ���� �Ŀ� �ε����� ī�����Ͽ� ���ο� ������ ����
	// render_particle: �������� �� �� ���ο��� �ε����� ī�����Ͽ� �ݺ��� ��ġ�� �������� ����

private:
	virtual void UpdateData() override;
};

