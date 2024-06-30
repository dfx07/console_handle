#pragma once

#include "console_type.h"
#include <vector>

/******************************************************************************/
/*ConsoleDevice*/

class ConsoleGraphics;
class ConsoleBoardView;

/* Flag for create device context */
#define DEVICE_CONTEXT_ANTIALIAS    0x0001
#define DEVICE_CONTEXT_USE_OPENGLEX 0x0002

/* Flag for DEVICEIP */
#define DEVICEIP_UPDATE_BOARD  0x0001
#define DEVICEIP_UPDATE_CUR    0x0002
#define DEVICEIP_UPDATE_COORD  0x0003

struct DeviceContextConfig
{
public:
	void SetAntiliasingLevel(int nLevel) {
		nFlag |= DEVICE_CONTEXT_ANTIALIAS;
		nAntialiasingLevel = nLevel;
	}

	void SetOpenGLExtension(bool bUse) {
		nFlag |= DEVICE_CONTEXT_USE_OPENGLEX;
	}

	void ClearFlag(unsigned int _nFlag)
	{
		nFlag &= ~_nFlag;
	}

	bool ValidFlag(int _nFlag) const noexcept { return nFlag & _nFlag; }
	int GetAntiliasingLevel() const noexcept { return nAntialiasingLevel; }

protected:
	int nFlag{ 0 };
	int nAntialiasingLevel{ 0 }; // 0~8
};

typedef enum tagConsoleDeviceEngine
{
	OPENGL,
	DIRECTX,
	GDIPLUS,

} ConsoleDeviceEngine;

interface DeviceContext
{
	virtual void* Render() = 0;
	virtual bool  IsValid() = 0;
	virtual bool  CreateContext(void* handle) = 0;
	virtual void  DeleteContext() = 0;
	virtual bool  MakeCurrentContext() = 0;
	virtual void  SwapBuffer() = 0;
};

interface ConsoleDeviceIP
{
	virtual void SetGraphics(ConsoleGraphics* pGraphic) = 0;
	virtual void SetFlags(int nflags) noexcept = 0;
	virtual void ClearFlags() noexcept = 0;
	virtual void RemoveFlags(int flag) noexcept = 0;
	virtual void AddFlags(int flag) noexcept = 0;
};

interface ConsoleDevice
{
	virtual bool Begin(ConsoleBoardView* pView) = 0;
	virtual void End() = 0;
	virtual void Draw() = 0;
	virtual void DrawBoard() = 0;
	virtual void Update() = 0;
	virtual void Clear() = 0;
};


/******************************************************************************/
/*ConsoleDrawBuffer*/

interface ConsoleDrawBufferIF
{
	virtual void OutText(ConsoleGpPoint pt, const WCHAR* str, ConsoleGpColor col) = 0;
	virtual void OutLine(ConsoleGpPoint pt1, ConsoleGpPoint pt2, ConsoleGpColor col) = 0;
	virtual void OutRectangle(ConsoleGpPoint pt, const float width, const float height, ConsoleGpColor col) = 0;
	virtual void OutTriangle(ConsoleGpPoint& pt1, ConsoleGpPoint& pt2, ConsoleGpPoint& pt3, ConsoleGpColor& col) = 0;
	virtual void OutPoint(ConsoleGpPoint& pt, const float radius, ConsoleGpColor& col) = 0;
};

class ConsoleDrawBuffer : public ConsoleDrawBufferIF
{
	typedef struct tagLine
	{
		ConsoleGpPoint pt1;
		ConsoleGpPoint pt2;
		ConsoleGpColor col;
	} LINE_DRAW, *PLINE_DRAW;

	typedef struct tagRect
	{
		ConsoleGpPoint pt;
		float width;
		float height;
		ConsoleGpColor col;
	} RECT_DRAW, *PRECT_DRAW;

	typedef struct tagText
	{
		ConsoleGpPoint pt;
		std::wstring str;
		ConsoleGpColor col;
	} TEXT_DRAW, *PTEXT_DRAW;

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
	virtual void OutText(ConsoleGpPoint pt, const WCHAR* str, ConsoleGpColor col)
	{
		int nIdx = GetNextIndex();
		m_Texts.push_back({ nIdx, TEXT_DRAW{pt, {str}, col } });
	}

	virtual void OutLine(ConsoleGpPoint pt1, ConsoleGpPoint pt2, ConsoleGpColor col)
	{
		int nIdx = GetNextIndex();
		m_Lines.push_back({ nIdx, LINE_DRAW{ pt1, pt2, col } });
	}

	virtual void OutRectangle(ConsoleGpPoint pt, const float width, const float height, ConsoleGpColor col)
	{
		int nIdx = GetNextIndex();
		m_Rects.push_back({ nIdx, RECT_DRAW{ pt, width, height, col } });
	}

	virtual void OutTriangle(ConsoleGpPoint& pt1, ConsoleGpPoint& pt2, ConsoleGpPoint& pt3, ConsoleGpColor& col)
	{
		int nIdx = GetNextIndex();
	}

	virtual void OutPoint(ConsoleGpPoint& pt, const float radius, ConsoleGpColor& col)
	{
		int nIdx = GetNextIndex();
	}

	std::vector<std::pair<int, LINE_DRAW>>& GetDrawBufferLines()
	{
		return m_Lines;
	}

	std::vector<std::pair<int, RECT_DRAW>>& GetDrawBufferRects()
	{
		return m_Rects;
	}

	void ClearDrawBuffer()
	{
		m_Lines.clear();
		m_Rects.clear();
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

	std::vector<std::pair<int, LINE_DRAW>> m_Lines;
	std::vector<std::pair<int, RECT_DRAW>> m_Rects;
	std::vector<std::pair<int, TEXT_DRAW>> m_Texts;
};