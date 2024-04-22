#pragma once

#include "console_type.h"

/******************************************************************************/
/*ConsoleDevice*/

class ConsoleGraphics;

#define DEVICE_CONTEXT_ANTIALIAS    0x0001
#define DEVICE_CONTEXT_USE_OPENGLEX 0x0002

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

	void ClearFlag(unsigned int flag)
	{
		nFlag &= ~flag;
	}

	bool ValidFlag(int flag) { return nFlag & flag; }
	int GetAntiliasingLevel() { return nAntialiasingLevel; }

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

interface ConsoleDevice
{
	virtual void Begin() = 0;
	virtual void End() = 0;
	virtual void Draw() = 0;
	virtual void Update() = 0;
	virtual void Clear() = 0;
};

interface ConsoleDeviceIO
{
	virtual void SetGraphics(ConsoleGraphics* pGraphic) = 0;
};


/******************************************************************************/
/*ConsoleDrawBuffer*/

interface ConsoleDrawBufferIF
{
	virtual void OutText(ConsoleGpPoint pt, const TCHAR* str, ConsoleGpColor col) = 0;
	virtual void OutLine(ConsoleGpPoint& pt1, ConsoleGpPoint& pt2, ConsoleGpColor& col) = 0;
	virtual void OutRectangle(ConsoleGpPoint pt, const float width, const float height, ConsoleGpColor col) = 0;
	virtual void OutTriangle(ConsoleGpPoint& pt1, ConsoleGpPoint& pt2, ConsoleGpPoint& pt3, ConsoleGpColor& col) = 0;
	virtual void OutPoint(ConsoleGpPoint& pt, const float radius, ConsoleGpColor& col) = 0;
};

class ConsoleDrawBuffer : public ConsoleDrawBufferIF
{
	struct LineTag
	{
		ConsoleGpPoint pt1;
		ConsoleGpPoint pt2;
		ConsoleGpColor col;
	};

	struct RectTag
	{
		ConsoleGpPoint pt;
		float width;
		float height;
		ConsoleGpColor col;
	};

public:

	virtual void OutText(ConsoleGpPoint pt, const TCHAR* str, ConsoleGpColor col)
	{
		nIndex++;
	}

	virtual void OutLine(ConsoleGpPoint& pt1, ConsoleGpPoint& pt2, ConsoleGpColor& col)
	{
		nIndex++;
		m_Lines.push_back({ nIndex, LineTag{ pt1, pt2, col } });
	}

	virtual void OutRectangle(ConsoleGpPoint pt, const float width, const float height, ConsoleGpColor col)
	{
		nIndex++;
		m_Rects.push_back({ nIndex, RectTag{ pt, width, height, col } });
	}

	virtual void OutTriangle(ConsoleGpPoint& pt1, ConsoleGpPoint& pt2, ConsoleGpPoint& pt3, ConsoleGpColor& col)
	{
		nIndex++;
	}

	virtual void OutPoint(ConsoleGpPoint& pt, const float radius, ConsoleGpColor& col)
	{
		nIndex++;
	}

	std::vector<std::pair<int, LineTag>>& GetDrawBufferLines()
	{
		return m_Lines;
	}

	std::vector<std::pair<int, RectTag>>& GetDrawBufferRects()
	{
		return m_Rects;
	}

protected:
	int nIndex{ 0 };

	std::vector<std::pair<int, LineTag>> m_Lines;
	std::vector<std::pair<int, RectTag>> m_Rects;
};