#pragma once


class CPathManager
	: public CSingleton<CPathManager>
{	
	SINGLETON(CPathManager);

private:
	wchar_t		m_szContentPath[256];

public:
	void init();
	
public:
	const wchar_t* GetContentPath() { return m_szContentPath; }
};

