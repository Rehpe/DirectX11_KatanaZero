#pragma once

class CEngine
	: public CSingleton<CEngine>
{
public:
	CEngine();
	~CEngine();

private:
	HWND	m_hWnd;			//윈도우 핸들
	Vec2	m_vResolution;	//윈도우 해상도

public:
	int init(HWND _hWnd, UINT iWidth, UINT _iHieght);
	void progress();

private:
	void tick();
	void render();

public:
	Vec2 GetWindowResolution() { return m_vResolution;}
	HWND GetMainWnd() { return m_hWnd; };
};

