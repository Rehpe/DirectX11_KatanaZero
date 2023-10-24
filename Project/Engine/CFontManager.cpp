#include "pch.h"
#include "CFontManager.h"
#include "CDevice.h"


CFontManager::CFontManager()
	: m_pFW1Factory(nullptr)
	, m_pFontWrapper(nullptr)
{
}

CFontManager::~CFontManager()
{
	if (nullptr != m_pFW1Factory)
		m_pFW1Factory->Release();

	if (nullptr != m_pFontWrapper)
		m_pFontWrapper->Release();
}

void CFontManager::init()
{
	if (FAILED(FW1CreateFactory(FW1_VERSION, &m_pFW1Factory)))
	{
		assert(NULL);
	}

	if (FAILED(m_pFW1Factory->CreateFontWrapper(DEVICE, L"Arial", &m_pFontWrapper)))
	{
		assert(NULL);
	}
}

void CFontManager::render()
{
	for (size_t i = 0; i < m_vecDebugFont.size(); i++)
	{
		tDebugFont fontInfo = m_vecDebugFont[i];

		DrawFont(fontInfo.szBuff, fontInfo.PosX, fontInfo.PosY, fontInfo.FontSize, fontInfo.Color);
	}

	m_vecDebugFont.clear();
}

void CFontManager::DrawFont(const wchar_t* _pStr, float _fPosX, float _fPosY, float _fFontSize, UINT _Color)
{
	m_pFontWrapper->DrawString(
		CONTEXT,
		_pStr, // String
		_fFontSize,// Font size
		_fPosX,// X position
		_fPosY,// Y position
		_Color,// Text color, 0xAaBbGgRr
		FW1_RESTORESTATE      // Flags (for example FW1_RESTORESTATE to keep context states unchanged)
	);
}

void CFontManager::AddDebugFont(const wchar_t* _pStr, float _fPosX, float _fPosY, float _fFontSize, UINT _Color)
{
	tDebugFont info = {};
	info.szBuff = _pStr;
	info.PosX = _fPosX;
	info.PosY = _fPosY;
	info.FontSize = _fFontSize;
	info.Color = _Color;
	
	m_vecDebugFont.push_back(info);
}
