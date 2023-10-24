#include "pch.h"
#include "CShader.h"

CShader::CShader(RES_TYPE _eType)
	: CResource(_eType, true)
	, m_ErrBlob(nullptr)
{
}

CShader::~CShader()
{
}
