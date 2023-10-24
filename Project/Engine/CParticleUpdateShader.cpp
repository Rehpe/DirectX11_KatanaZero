#include "pch.h"
#include "CParticleUpdateShader.h"

#include "CStructuredBuffer.h"

CParticleUpdateShader::CParticleUpdateShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY, UINT _iGroupPerThreadZ)
	:m_ParticleBuffer(nullptr)
	, m_ModuleData(nullptr)
	, m_RWBuffer(nullptr)
{
	m_iGroupPerThreadX = _iGroupPerThreadX;
	m_iGroupPerThreadY = _iGroupPerThreadY;
	m_iGroupPerThreadZ = _iGroupPerThreadZ;
}

CParticleUpdateShader::~CParticleUpdateShader()
{
}

void CParticleUpdateShader::SetParticleBuffer(CStructuredBuffer* _Buffer)
{
	// ��ƼŬ ���� ���
	m_ParticleBuffer = _Buffer;

	// ��ƼŬ ���� ������ۿ� ����
	m_Const.arrInt[0] = m_ParticleBuffer->GetElementCount();
}

void CParticleUpdateShader::UpdateData()
{
	// ��ƼŬ ���� u0 �������Ϳ� ���ε� 

	// ��ƼŬ�� ��ġ���� ���̳� ������ ��ƼŬ ������ �ǽð����� �����Ǿ��(CS�� ���Ⱑ �����ؾ�)�ϹǷ�
	// 2��°���� false(UAV ���ε�)
	m_ParticleBuffer->UpdateData_CS(0, false);	
	m_RWBuffer->UpdateData_CS(1, false);

	// ��ⵥ������ ��� CS�� �ǽð����� �����ϴ� ���� �ƴϹǷ�, true(SRV ���ε�)
	m_ModuleData->UpdateData_CS(20, true);

	// ������ �ؽ�ó�� ��� ������ ���ø������� ����ϹǷ�(�б�����) true(SRV ���ε�)
	m_NoiseTex->UpdateData_CS(21, true);

	// �׷� �� (��ƼŬ ���� / �׷캰 ������ �� + 1(�������� ����Ͽ�))
	m_iGroupX = (m_ParticleBuffer->GetElementCount() / m_iGroupPerThreadX) + 1;
}

void CParticleUpdateShader::Clear()
{
	// ��ƼŬ ���� u�������Ϳ��� Ŭ����
	m_ParticleBuffer->Clear_CS(false);
	m_RWBuffer->Clear_CS(false);
	m_ModuleData->Clear_CS(true);
	m_NoiseTex->Clear_CS(true);
}

