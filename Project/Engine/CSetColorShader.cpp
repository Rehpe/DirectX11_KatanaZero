#include "pch.h"
#include "CSetColorShader.h"

#include "CTexture.h"

CSetColorShader::CSetColorShader(UINT _iGroupPerThreadX, UINT _iGroupPerThreadY, UINT _iGroupPerThreadZ)
{
	// �������� ���ڷ� ���� �׷� �� ������ ������ ����� ����
	m_iGroupPerThreadX = _iGroupPerThreadX;
	m_iGroupPerThreadY = _iGroupPerThreadY;
	m_iGroupPerThreadZ = _iGroupPerThreadZ;
}

CSetColorShader::~CSetColorShader()
{
}

void CSetColorShader::UpdateData()
{
	// �ؽ�ó�� u0 �������Ϳ� ���ε��Ѵ�
	m_OutTex->UpdateData_CS(0, false);

	// �׷� ���� ��� (�ػ� % ������ ����)
	m_iGroupX = m_OutTex->Width() / m_iGroupPerThreadX;
	m_iGroupY = m_OutTex->Height() / m_iGroupPerThreadY;
	m_iGroupZ = 1;
}

void CSetColorShader::Clear()
{
	m_OutTex->Clear();
}
