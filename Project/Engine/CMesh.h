#pragma once
#include "CResource.h"

//정점들의 정보를 리소스화시켜 관리하는 클래스

class CMesh :
    public CResource
{
public:
    CMesh(bool _bEngine = false);
    ~CMesh();

private:
	ComPtr<ID3D11Buffer>		m_VB;			//정점 버퍼
	D3D11_BUFFER_DESC			m_tVBDesc;
	UINT						m_VtxCount;

	ComPtr<ID3D11Buffer>		m_IB;			//인덱스 버퍼
	D3D11_BUFFER_DESC			m_tIBDesc;
	UINT						m_IdxCount;

	D3D11_PRIMITIVE_TOPOLOGY	m_Topology;

	void* m_pVtxSys;						//GPU와 대응시킬 정점 버퍼 시스템 메모리
	void* m_pIdxSys;						//GPU와 대응시킬 인덱스 버퍼 시스템 메모리


	// 일단 순수가상함수 푸는 용으로 더미로 구현해놓겠다.
public:
	virtual int Save(const wstring& _strRelativePath) { return S_OK; }
private:
	virtual int Load(const wstring& _strFilePath) { return S_OK; }
   
public:
	void Create(void* _VtxSysMem, UINT _iVtxCount, void* _IdxSysMem, UINT _IdxCount);
	void CreateLine(void* _VtxSysMem, UINT _iVtxCount, void* _IdxSysMem, UINT _IdxCount);


	void render();
	void render_particle(UINT _iParticleCount);


	// render와 render_particle의 차이
	// render: 하나의 파이프라인(IA->VS->...OM) 이 모두 끝난 후에 인덱스를 카운팅하여 새로운 렌더링 시작
	// render_particle: 렌더링에 들어간 후 내부에서 인덱스를 카운팅하여 반복을 마치고 렌더링을 종료

private:
	virtual void UpdateData() override;
};

