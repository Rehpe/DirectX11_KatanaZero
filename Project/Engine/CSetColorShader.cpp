#include "pch.h"
#include "CSetColorShader.h"

#include "CTexture.h"

CSetColorShader::CSetColorShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY, UINT _iGroupPerThreadZ)
{
	// 생성자의 인자로 들어온 그룹 당 쓰레드 개수를 멤버에 저장
	m_iGroupPerThreadX = _iGroupPerThreadX;
	m_iGroupPerThreadY = _iGroupPerThreadY;
	m_iGroupPerThreadZ = _iGroupPerThreadZ;
}

CSetColorShader::~CSetColorShader()
{
}

void CSetColorShader::UpdateData()
{
	// 텍스처를 u0 레지스터에 바인딩한다
	m_OutTex->UpdateData_CS(0, false);

	// 그룹 개수 계산 (해상도 % 쓰레드 개수)
	m_iGroupX = m_OutTex->Width() / m_iGroupPerThreadX;
	m_iGroupY = m_OutTex->Height() / m_iGroupPerThreadY;
	m_iGroupZ = 1;
}

void CSetColorShader::Clear()
{
	m_OutTex->Clear();
}
