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
	// 파티클 버퍼 등록
	m_ParticleBuffer = _Buffer;

	// 파티클 개수 상수버퍼에 전달
	m_Const.arrInt[0] = m_ParticleBuffer->GetElementCount();
}

void CParticleUpdateShader::UpdateData()
{
	// 파티클 버퍼 u0 레지스터에 바인딩 

	// 파티클의 위치정보 등이나 스폰된 파티클 개수는 실시간으로 수정되어야(CS로 쓰기가 가능해야)하므로
	// 2번째인자 false(UAV 바인딩)
	m_ParticleBuffer->UpdateData_CS(0, false);	
	m_RWBuffer->UpdateData_CS(1, false);

	// 모듈데이터의 경우 CS가 실시간으로 수정하는 것이 아니므로, true(SRV 바인딩)
	m_ModuleData->UpdateData_CS(20, true);

	// 노이즈 텍스처의 경우 오로지 샘플링용으로 사용하므로(읽기전용) true(SRV 바인딩)
	m_NoiseTex->UpdateData_CS(21, true);

	// 그룹 수 (파티클 개수 / 그룹별 쓰레드 수 + 1(나머지를 고려하여))
	m_iGroupX = (m_ParticleBuffer->GetElementCount() / m_iGroupPerThreadX) + 1;
}

void CParticleUpdateShader::Clear()
{
	// 파티클 버퍼 u레지스터에서 클리어
	m_ParticleBuffer->Clear_CS(false);
	m_RWBuffer->Clear_CS(false);
	m_ModuleData->Clear_CS(true);
	m_NoiseTex->Clear_CS(true);
}

