#pragma once

#include <assert.h>
#include "console_device.h"
#include "console_model.h"
#include "console_view.h"

#pragma warning( disable : 26812)

class ConsoleHandle;
class ConsoleGraphics;

#define ON_FUNCTION_WINDOW(bFunCheck, ...)\
{\
    if(bFunCheck) \
    {\
        return bFunCheck(__VA_ARGS__);\
    }\
}

#define assert_msg(cond, fmt) assert(cond && fmt)

enum ConsoleEvent
{
	MOUSE_CONSOLE_EVENT,
	KEYBOARD_CONSOLE_EVENT,
	NONE_CONSOLE_EVENT,
};

enum ConsoleMouseState
{
	MOUSE_NONE_STATE = 0x0000,
	MOUSE_MOVE_STATE = 0x0001,
	MOUSE_MOVE_NO_OVER_STATE = 0x0002,
	MOUSE_UP_STATE = 0x0004,
	MOUSE_DOWN_STATE = 0x0008,
};

enum ConsoleKeyboardState
{
	KEYBOARD_NONE_STATE = 0x0000,
	KEYBOARD_DOWN_STATE = 0x0001,
	KEYBOARD_UP_STATE = 0x0002,
};

enum ConsoleMouseButton
{
	MOUSE_BUTTON_NONE,
	MOUSE_BUTTON_LEFT,
	MOUSE_BUTTON_MID,
	MOUSE_BUTTON_RIGHT,
};

struct ConsoleMousePos
{
	int x{ 0 };
	int y{ 0 };
};

struct MouseEventInfo
{
	ConsoleMouseButton	m_MouseButton{ MOUSE_BUTTON_NONE };
	int					m_MouseState{ 0 };
	ConsoleMousePos		m_MousePos;
};

struct KeyBoardEventInfo
{
	int m_nKey{ 0 };
	int m_nState{ 0 };
};

// Console handle
interface ConsoleHandleBase
{
	virtual bool Create(const TCHAR* strTitle, unsigned int nWidth, const int nHeight) = 0;
	virtual void SetWindowSize(const int nRow, const int nCol) = 0;
	virtual void SetWindowPosition(const int xPos, const int yPos) = 0;
	virtual void SetCellSize(const int nWidth, const int nHeight) = 0;
	virtual void ShowCursor(bool bShow) = 0;
	virtual void Show() = 0;
	virtual void Hide() = 0;
	virtual void SetFont(const TCHAR* strFont) = 0;
	virtual void SetTitle(const TCHAR* strTitle) = 0;
	virtual bool Closed() const = 0;
};

// Handle Event
interface ConsoleHandleEvent
{
protected:
	typedef void(*typeFunOnMouseEvent) (ConsoleHandle* handle, MouseEventInfo* mouseInfo);
	typedef void(*typeFunOnKeyboardEvent)(ConsoleHandle* handle, KeyBoardEventInfo* keyboardInfo);
	typedef void(*typeFunOnResizeEvent) (ConsoleHandle* handle);
	typedef void(*typeFunOnDraw) (ConsoleHandle* handle, ConsoleGraphics* pView);

public:
	virtual void OnMouseEvent() = 0;
	virtual void OnKeyBoard() = 0;
	virtual void OnResize() = 0;
	virtual void OnDraw() = 0;

public:
	virtual void PollEvent() = 0;
	virtual void WaitEvent() = 0;
	virtual MouseEventInfo* GetMouseEvent() = 0;
	virtual KeyBoardEventInfo* GetKeyboardEvent() = 0;
};

// Console graphic base
interface ConsoleGraphicsBase
{
public:
	virtual void Clear() = 0;
	virtual void DrawText(const int r, const int c, const TCHAR* str) = 0;
	virtual void DrawCellColor(const int r, const int c, float colr, float colg, float colb) = 0;

	virtual void SetDevice(ConsoleDevice* pDevice) { m_pDevice = pDevice; }
	virtual void SetModelData(ConsoleBoardModelData* pModelData) { m_pModelData = pModelData; }

protected:
	ConsoleBoardModelData* m_pModelData{ nullptr };
	ConsoleDevice* m_pDevice{ nullptr };
};

// Console graphic
class ConsoleGraphics : public ConsoleGraphicsBase
{
public:
	virtual void Clear()
	{
		if (!m_pDevice)
			return;

		m_pDevice->Clear();
	}

	virtual void DrawText(const int r, const int c, const TCHAR* str)
	{
		ConsoleCellDraw* pCellDraw = m_pModelData->GetCell(r, c);

		if (!pCellDraw)
			return;

		float x = pCellDraw->m_fX;
		float y = pCellDraw->m_fY;

		m_DrawBuffer.OutText(ConsoleGpPoint{ x, y }, _T("1"), ConsoleGpColor{ 255.f, 255.f, 255.f });
	}

	virtual void DrawCellColor(const int r, const int c, float colr, float colg, float colb)
	{
		ConsoleCellDraw* pCellDraw = m_pModelData->GetCell(r, c);

		if (!pCellDraw)
			return;

		float x = pCellDraw->m_fX;
		float y = pCellDraw->m_fY;

		float width = pCellDraw->m_fWidth;
		float height = pCellDraw->m_fHeight;

		m_DrawBuffer.OutRectangle(ConsoleGpPoint{ x, y }, width, height, ConsoleGpColor{ colr, colg, colb });
	}

protected:
	ConsoleDrawBuffer	m_DrawBuffer;
};

class ConsoleHandle : public ConsoleHandleBase, public ConsoleHandleEvent
{
protected:
	virtual void OnMouseEvent() { ON_FUNCTION_WINDOW(m_funOnMouseEvent, this, &m_MouseEvent) }
	virtual void OnKeyBoard() { ON_FUNCTION_WINDOW(m_funOnKeyboardEvent, this, &m_KeyboardEvent) }
	virtual void OnResize() { ON_FUNCTION_WINDOW(m_funOnResizeEvent, this) }
	virtual void OnDraw() { ON_FUNCTION_WINDOW(m_funOnDraw, this, m_View.GetGraphics()) }

public:
	void SetMouseEventCallback(typeFunOnMouseEvent funCallBack) { m_funOnMouseEvent = funCallBack; }
	void SetKeyboardEventCallback(typeFunOnKeyboardEvent funCallBack) { m_funOnKeyboardEvent = funCallBack; }
	void SetResizeEventCallback(typeFunOnResizeEvent funCallBack) { m_funOnResizeEvent = funCallBack; }
	void SetDrawCallback(typeFunOnDraw funCallBack) { m_funOnDraw = funCallBack; }

public:
	virtual void* GetHandle() = 0;
	virtual bool Create(const TCHAR* strTitle, unsigned int nWidth, const int nHeight) = 0;
	virtual void SetWindowSize(const int nRow, const int nCol) = 0;
	virtual void SetWindowPosition(const int xPos, const int yPos) = 0;
	virtual void SetCellSize(const int nWidth, const int nHeight) = 0;
	virtual void ShowCursor(bool bShow) = 0;
	virtual void Show() = 0;
	virtual void Hide() = 0;
	virtual void SetFont(const TCHAR* strFont) = 0;
	virtual void SetTitle(const TCHAR* strTitle) = 0;
	virtual bool Closed() const = 0;

	virtual void PollEvent() = 0;
	virtual void WaitEvent() = 0;
	virtual MouseEventInfo* GetMouseEvent() { return &m_MouseEvent; }
	virtual KeyBoardEventInfo* GetKeyboardEvent() { return &m_KeyboardEvent; }

	virtual void Draw() = 0;

public:
	ConsoleHandle() { }
	virtual ~ConsoleHandle() { }

protected:
	virtual bool CreateBoardView(const unsigned int nWidth, const unsigned int nHeight) = 0;
	virtual bool CreateBoardModel(const int nRow, const int nCol) = 0;

protected:
	typeFunOnMouseEvent m_funOnMouseEvent{ nullptr };
	typeFunOnKeyboardEvent m_funOnKeyboardEvent{ nullptr };
	typeFunOnResizeEvent m_funOnResizeEvent{ nullptr };
	typeFunOnDraw m_funOnDraw{ nullptr };

protected:
	bool					m_bClosed{ false };
	unsigned int			m_uWidth{ 0 };
	unsigned int			m_uHeight{ 0 };

	ConsoleBoardModelData	m_ModelData;
	ConsoleBoardView		m_View;
	ConsoleDevice*			m_pDevice{ nullptr };

	MouseEventInfo			m_MouseEvent;
	KeyBoardEventInfo		m_KeyboardEvent;
};
