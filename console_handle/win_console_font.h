#pragma once

#include "console_font.h"

#include <Windows.h>

///////////////////////////////////////////////////////////////////////////////
/*WinConsoleFont class*/

class WinConsoleFont : public ConsoleFont
{
public:
	WinConsoleFont() : m_hFont(NULL)
	{
		m_dwFontType = 0L;
		m_nFontSize = 10;
	}

	~WinConsoleFont()
	{
		UnLoad();
	}

protected:
	bool CreateConsoleFont()
	{
		if (m_hFont)
			::DeleteObject(m_hFont);

		unsigned int nHeight = GetHeightFixel();

		m_hFont = ::CreateFont(nHeight, 0, 0, 0, m_dwFontType, FALSE, FALSE, FALSE,
							DEFAULT_CHARSET, OUT_TT_PRECIS,
							CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
							FF_DONTCARE | DEFAULT_PITCH, m_strFontName.c_str());

		return !!m_hFont;
	}

	unsigned int GetHeightFixel() const
	{
		HDC hDC = GetDC(NULL);
		LONG lHeight = -MulDiv(m_nFontSize, ::GetDeviceCaps(hDC, LOGPIXELSY), 72);

		::ReleaseDC(NULL, hDC);

		return lHeight;
	}

public:
	static unsigned int GetFontSizeFrom(HDC hDC, HFONT hfont, unsigned int nheight)
	{
		auto oldHfont = ::SelectObject(hDC, hfont);

		unsigned int nFontSize = 0;

		int dpi = GetDeviceCaps(hDC, LOGPIXELSY);
		nFontSize = (nheight * 72) / dpi;

		::SelectObject(hDC, oldHfont);

		return nFontSize;
	}

public:

	virtual void* GetHandle() noexcept
	{
		if (!m_hFont)
			return NULL;

		return &m_hFont;
	}

	virtual bool Load(const ConsoleString font_name, unsigned int font_size, ConsoleFontType font_type)
	{
		m_dwFontType = FW_REGULAR;

		switch (font_type)
		{
			case ConsoleFontType::Bold:
				m_dwFontType = FW_BOLD;
				break;
			case ConsoleFontType::Thin:
				m_dwFontType = FW_THIN;
				break;
			default:
				break;
		}

		m_strFontName = font_name;
		m_nFontSize = font_size;

		return CreateConsoleFont();
	}

	virtual void UnLoad() noexcept
	{
		if (m_hFont)
			::DeleteObject(m_hFont);
	}

	virtual void ChangeSize(unsigned int font_size) noexcept
	{
		// Retrieve the LOGFONT structure of the current font
		LOGFONT logfont;
		GetObject(m_hFont, sizeof(LOGFONT), &logfont);

		HDC hDC = GetDC(NULL);

		// Modify the font size (lfHeight)
		logfont.lfHeight = -MulDiv(font_size, ::GetDeviceCaps(hDC, LOGPIXELSY), 72);

		// Create a new font with the modified LOGFONT structure
		HFONT hNewFont = CreateFontIndirect(&logfont);

		::ReleaseDC(NULL, hDC);

		if (hNewFont)
		{
			UnLoad();
			m_hFont = hNewFont;
			m_nFontSize = font_size;
		}
	}

	virtual ConsoleFontPtr CreateConsoleFontIndirect(const ConsoleString font_name, unsigned int font_size, ConsoleFontType font_type) noexcept
	{
		auto pWinFont = std::make_shared<WinConsoleFont>();

		if (!pWinFont->Load(font_name, font_size, font_type))
		{
			assert(0);

			return nullptr;
		}

		return pWinFont;
	}

	virtual ConsoleFontPtr CreateConsoleFontIndirect(const ConsoleFontKey& fontKey, ConsoleFontType font_type/* = ConsoleFontType::Normal*/) noexcept
	{
		return CreateConsoleFontIndirect(fontKey.name, fontKey.size, font_type);
	}

	virtual ConsoleFont* Clone()
	{
		if (!m_hFont)
			return NULL;

		WinConsoleFont* pWinNewFont = new WinConsoleFont();

		// Retrieve the LOGFONT structure of the current font
		LOGFONT logfont;
		GetObject(m_hFont, sizeof(LOGFONT), &logfont);

		*pWinNewFont = *this;
		HFONT hNewFont = CreateFontIndirect(&logfont);

		if (hNewFont)
		{
			pWinNewFont->m_hFont = hNewFont;
			return pWinNewFont;
		}

		delete pWinNewFont;
		return NULL;
	}

	virtual ConsoleString GetFontName() noexcept
	{
		return m_strFontName;
	}

	virtual unsigned int GetFontSize() noexcept
	{
		return m_nFontSize;
	}

protected:
	HFONT			m_hFont{ NULL };

	ConsoleString	m_strFontName;
	unsigned int	m_nFontSize;
	DWORD			m_dwFontType;
};
