////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************
* Copyright (C) 2023-2024 thuong.nv <thuong.nv.mta@gmail.com>
* MIT software Licencs, see the accompanying
* http://www.opensource.org/licenses/mit-license.php
*
/***********************************************************************************
* @brief : Console device
* @file  : console_device
* @create: Aug 17, 2024
* @note  : For conditions of distribution and use, see copyright notice in readme.txt
***********************************************************************************/

#pragma once

#include "console_type.h"
#include <vector>
#include <map>

#include "console_font.h"

#include "gl_render_def.h"

/******************************************************************************/
/*ConsoleDevice*/

class ConsoleGraphics;

/* Flag for create device context */
#define DEVICE_CONTEXT_ANTIALIAS    0x0001
#define DEVICE_CONTEXT_USE_OPENGLEX 0x0002

/* Flag for DEVICEIP */
#define DEVICEIP_UPDATE_BOARD  0x0001
#define DEVICEIP_UPDATE_CUR    0x0002
#define DEVICEIP_UPDATE_COORD  0x0003


typedef enum tagConsoleDeviceEngine
{
	OPENGL,
	DIRECTX,
	GDIPLUS,

} ConsoleDeviceEngine;

/////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************/
// ConsoleDeviceControl class

class ConsoleDeviceControl : public ConsoleRenderControl
{
public:
	ConsoleDeviceControl()
	{
		m_pViewInfo = std::make_shared<ConsoleViewInfo>();
	}

public:
	virtual void SetFontManager(ConsoleFontManagerPtr pFontMana) noexcept
	{
		m_pFontMana = pFontMana;
	}

	virtual ConsoleFontManagerPtr GetFontManager() const noexcept
	{
		return m_pFontMana;
	}

	virtual void SetFlags(const int nflags) noexcept
	{
		m_nFlags = nflags;
	}

	virtual void ClearFlags() noexcept
	{
		m_nFlags = 0;
	}

	virtual void RemoveFlags(const int flag) noexcept
	{
		m_nFlags &= ~flag;
	}

	virtual void AddFlags(const int flag) noexcept
	{
		m_nFlags |= flag;
	}

	virtual bool ValidFlags(const int nFlags) noexcept {
		return !!(m_nFlags & nFlags);
	}

public:
	unsigned int			m_nFlags{ 0 };
	ConsoleFontManagerPtr	m_pFontMana{ nullptr };
};

/////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************/
// ConsoleDevice class

class ConsoleDevice
{
public:
	virtual bool Begin(ConsoleView* pView) = 0;
	virtual void End() = 0;
	virtual void Draw() = 0;
	virtual void Update() = 0;
	virtual void Clear() = 0;

public:
	ConsoleDevice() {
		m_pDeviceCtrl = std::make_shared<ConsoleDeviceControl>();
	}

	virtual ~ConsoleDevice() {}

public:
	ConsoleDeviceControl* GetDeviceControl() const noexcept {
		return m_pDeviceCtrl.get();
	}

protected:
	ConsoleDeviceControlPtr m_pDeviceCtrl;
};


/******************************************************************************/
/*ConsoleDrawBuffer*/

interface ConsoleDrawBufferIF
{
	virtual void OutText(ConsolePoint pt, const ConsoleString& str, ConsoleColor col) = 0;
	virtual void OutLine(ConsolePoint pt1, ConsolePoint pt2, ConsoleColor col) = 0;
	virtual void OutRectangle(ConsolePoint pt, const float width, const float height, ConsoleColor col) = 0;
	virtual void OutTriangle(ConsolePoint& pt1, ConsolePoint& pt2, ConsolePoint& pt3, ConsoleColor& col) = 0;
	virtual void OutPoint(ConsolePoint& pt, const float radius, ConsoleColor& col) = 0;
};

class ConsoleDrawBuffer : public ConsoleDrawBufferIF
{
	enum
	{
		RESERVE_DRAW_BUFFER_DATA = 1000,
	};

public:
	enum BUFFER_TYPE {
		LINE = 0x0001,
		RECT = 0x0002,
		TEXT = 0x0004,
	};

public:
	typedef struct tagLine
	{
		ConsolePoint pt1;
		ConsolePoint pt2;
		ConsoleColor col;
	} LINE_DRAW, *PLINE_DRAW;

	typedef struct tagRect
	{
		ConsolePoint pt;
		float width;
		float height;
		ConsoleColor col;
	} RECT_DRAW, *PRECT_DRAW;

	typedef struct tagText
	{
		ConsolePoint pt;
		std::wstring str;
		ConsoleColor col;
	} TEXT_DRAW, *PTEXT_DRAW;

public:
	typedef std::vector<std::pair<int, LINE_DRAW>> VEC_LINE_DRAW_DATA;
	typedef std::vector<std::pair<int, RECT_DRAW>> VEC_RECT_DRAW_DATA;
	typedef std::vector<std::pair<int, TEXT_DRAW>> VEC_TEXT_DRAW_DATA;
	typedef std::map<ConsoleFontKey, VEC_TEXT_DRAW_DATA> MAP_TEXT_DRAW_DATA;

protected:
	int GetNextIndex() noexcept
	{
		if (m_bSkipIndex)
		{
			return m_nIndex;
		}

		return ++m_nIndex;
	}

public:
	ConsoleDrawBuffer()
	{
		m_FontKeyDefault = { _T("Arial"), 12 };

		m_Lines.reserve(RESERVE_DRAW_BUFFER_DATA);
		m_Rects.reserve(RESERVE_DRAW_BUFFER_DATA);
	}

	void SetDefaultFont(const wchar_t* font_name, const int font_size)
	{
		m_FontKeyDefault = ConsoleFontKey{ font_name, (unsigned int)font_size };
	}

	virtual void OutText(ConsolePoint pt, const ConsoleString& str, ConsoleColor col)
	{
		int nIdx = GetNextIndex();
		auto itFound = m_Texts.insert(std::make_pair(m_FontKeyDefault,
			VEC_TEXT_DRAW_DATA{ { nIdx, TEXT_DRAW{ pt, {str}, col } }}));

		if (itFound.second == false)
		{
			itFound.first->second.push_back({ nIdx, TEXT_DRAW{ pt, {str}, col } });
		}
	}

	virtual void OutText(ConsolePoint pt, const ConsoleString& str, ConsoleColor col, ConsoleFontKey font_key)
	{
		int nIdx = GetNextIndex();
		auto itFound = m_Texts.insert(std::make_pair(font_key, 
			VEC_TEXT_DRAW_DATA{ { nIdx, TEXT_DRAW{ pt, {str}, col } } }));

		if (itFound.second == false)
		{
			itFound.first->second.push_back({ nIdx, TEXT_DRAW{ pt, {str}, col } });
		}
	}

	virtual void OutLine(ConsolePoint pt1, ConsolePoint pt2, ConsoleColor col)
	{
		int nIdx = GetNextIndex();
		m_Lines.push_back({ nIdx, LINE_DRAW{ pt1, pt2, col } });
	}

	virtual void OutRectangle(ConsolePoint pt, const float width, const float height, ConsoleColor col)
	{
		int nIdx = GetNextIndex();
		m_Rects.push_back({ nIdx, RECT_DRAW{ pt, width, height, col } });
	}

	virtual void OutTriangle(ConsolePoint& pt1, ConsolePoint& pt2, ConsolePoint& pt3, ConsoleColor& col)
	{
		int nIdx = GetNextIndex();
	}

	virtual void OutPoint(ConsolePoint& pt, const float radius, ConsoleColor& col)
	{
		int nIdx = GetNextIndex();
	}

	VEC_LINE_DRAW_DATA& GetLinesDrawBuffer() noexcept
	{
		return m_Lines;
	}

	VEC_RECT_DRAW_DATA& GetRectsDrawBuffer() noexcept
	{
		return m_Rects;
	}

	MAP_TEXT_DRAW_DATA& GetTextsDrawBuffer() noexcept
	{
		return m_Texts;
	}

	void ClearDrawBuffer(int nFlag = 0xffff)
	{
		if(nFlag & BUFFER_TYPE::LINE)
			m_Lines.clear();

		if (nFlag & BUFFER_TYPE::RECT)
			m_Rects.clear();

		if (nFlag & BUFFER_TYPE::TEXT)
			m_Texts.clear();

		m_nIndex = 0;
	}

public:
	void SkipIncreaseIndex(const bool bSkip) noexcept
	{
		m_bSkipIndex = bSkip;
	}

	bool IsSkipIncreaseIndex() const noexcept
	{
		return m_bSkipIndex;
	}

	void IncreaseIndex(const int nFac = 1) noexcept
	{
		m_nIndex += nFac;
	}



protected:
	bool m_bSkipIndex{ false };
	int  m_nIndex{ 0 };

	ConsoleFontKey		m_FontKeyDefault;

	VEC_LINE_DRAW_DATA	m_Lines;
	VEC_RECT_DRAW_DATA	m_Rects;
	MAP_TEXT_DRAW_DATA	m_Texts;
};