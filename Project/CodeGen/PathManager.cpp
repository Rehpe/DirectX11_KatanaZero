#include "pch.h"
#include "PathManager.h"

wchar_t CPathManager::g_szSolutionPath[255] = L"";
wchar_t CPathManager::g_szResPath[255] = L"";
wchar_t CPathManager::g_szIncPath[255] = L"";
wchar_t CPathManager::g_szProjPath[255] = L"";
wchar_t CPathManager::g_szRelativePath[255] = L"";
wchar_t CPathManager::g_szFile[255] = L"";
wchar_t CPathManager::g_szExt[50] = L"";

void CPathManager::init()
{
	// Resources 폴더까지의 경로를 구한다.
	GetCurrentDirectory(255, g_szSolutionPath);
	
	size_t iCount = 0;
	for (size_t i = wcslen(g_szSolutionPath) - 1; i > 0; --i)
	{
		if ('\\' == g_szSolutionPath[i])
		{
			g_szSolutionPath[i + 1] = 0;
			++iCount;
		}

		if (iCount == 2)
			break;
	}

	// SolutionPath -> copy -> ResPath
	wcscpy_s(g_szResPath, 255, g_szSolutionPath);

	// -> bin -> Resources
	wcscat_s(g_szResPath, L"OutputFile\\bin\\content\\");
	
	// Inc 폴더 경로 
	GetCurrentDirectory(255, g_szIncPath);
	
	iCount = 0;
	for (size_t i = wcslen(g_szIncPath) - 1; i > 0; --i)
	{
		if ('\\' == g_szIncPath[i])
		{
			g_szIncPath[i + 1] = 0;
			++iCount;
		}

		if (iCount == 2)
			break;
	}

	wcscat_s(g_szIncPath, L"External\\Include\\");


	// Proj 폴더 경로 
	GetCurrentDirectory(255, g_szProjPath);

	iCount = 0;
	for (size_t i = wcslen(g_szProjPath) - 1; i > 0; --i)
	{
		if ('\\' == g_szProjPath[i])
		{
			g_szProjPath[i + 1] = 0;
			++iCount;
		}

		if (iCount == 2)
			break;
	}

	wcscat_s(g_szProjPath, L"Project\\");
}

wchar_t * CPathManager::GetResPath()
{
	return g_szResPath;
}

wchar_t * CPathManager::GetIncludePath()
{
	return g_szIncPath;
}

wchar_t * CPathManager::GetProjectPath()
{
	return g_szProjPath;
}

wchar_t * CPathManager::GetFileName(const wchar_t * _strPath)
{
	_wsplitpath_s(_strPath, NULL, 0, NULL, 0, g_szFile, 255, NULL, 0);
	return g_szFile;
}

wchar_t * CPathManager::GetExt(const wchar_t * _strPath)
{
	_wsplitpath_s(_strPath, NULL, 0, NULL, 0, nullptr, 0, g_szExt, 50);
	return g_szExt;
}

wchar_t * CPathManager::GetRelativePath(const wchar_t * _pFullPath)
{
	wmemset(g_szRelativePath, 0, 255);

	wstring str = _pFullPath;
	size_t iLen = wcslen(g_szResPath);
	str = str.substr(iLen, str.length() - iLen).c_str();
	wcscpy_s(g_szRelativePath, 255, str.c_str());
	return g_szRelativePath;
}

wchar_t * CPathManager::GetSolutionPath()
{
	return g_szSolutionPath;
}
