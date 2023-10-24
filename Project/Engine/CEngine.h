#pragma once

class CEngine
	: public CSingleton<CEngine>
{
public:
	CEngine();
	~CEngine();

private:
	HWND	m_hWnd;			//������ �ڵ�
	Vec2	m_vResolution;	//������ �ػ�

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

