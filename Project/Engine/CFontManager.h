#pragma once
#include "CSingleton.h"

#include <FontEngine/FW1FontWrapper.h>
#include <FontEngine/FW1CompileSettings.h>

#ifdef _DEBUG
#pragma comment(lib, "FontEngine/FW1FontWrapper_debug")
#else
#pragma comment(lib, "FontEngine/FW1FontWrapper"))
#endif


#define FONT_RGBA(r, g, b, a) (((((BYTE)a << 24 ) | (BYTE)b << 16) | (BYTE)g << 8) | (BYTE)r)

class CFontManager
	: public CSingleton<CFontManager>
{
private:
	IFW1Factory* m_pFW1Factory;
	IFW1FontWrapper* m_pFontWrapper;

	vector<tDebugFont>	m_vecDebugFont;

public:
	void init();
	void render();

	void DrawFont(const wchar_t* _pStr, float _fPosX, float _fPosY, float _fFontSize, UINT _Color);

	void AddDebugFont(const wchar_t* _pStr, float _fPosX, float _fPosY, float _fFontSize, UINT _Color);

public:
	CFontManager();
	~CFontManager();
};
