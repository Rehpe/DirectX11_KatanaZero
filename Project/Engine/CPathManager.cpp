#include "pch.h"
#include "CPathManager.h"

#include "CEngine.h"

CPathManager::CPathManager()
	: m_szContentPath{}
{
}

CPathManager::~CPathManager()
{
}


void CPathManager::init()
{
	GetCurrentDirectory(256, m_szContentPath);	

	// ���������� ��
	int iLen = (int)wcslen(m_szContentPath);

	for (int i = iLen - 1; i >= 0; --i)
	{
		if (L'\\' == m_szContentPath[i])
		{
			m_szContentPath[i] = 0;
			break;
		}
	}

	// + bin\\content
	wcscat_s(m_szContentPath, L"\\bin\\content\\");		
}