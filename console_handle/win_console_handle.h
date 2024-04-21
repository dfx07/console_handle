#pragma once

#include <Windows.h>

#include "console_handle.h"
#include "console_view.h"

class WinConsoleHandle : public ConsoleHandle, public ConsoleHandleEvent
{
	struct WinConsoleStyle
	{
		DWORD m_dwStyle;
		DWORD m_dwStyleEx;
	};

	struct WinConsoleState
	{
		int xpos;
		int ypos;
		unsigned int width;
		unsigned int height;
	};

protected:
	HWND CreateConsoleHandle(const TCHAR* strWndClassName, const TCHAR* title, WinConsoleState& state, WinConsoleStyle& style)
	{
		// adjust the size
		RECT wr = { 0, 0, (LONG)state.width, (LONG)state.height };
		AdjustWindowRect(&wr, style.m_dwStyle, FALSE);

		m_hWnd = CreateWindowEx(style.m_dwStyleEx,//
			strWndClassName,                      //
			title,                                //
			m_dwStyle,                            //
			m_CurStatus.m_rect.x,                 //
			m_CurStatus.m_rect.y,                 // Postion 
			wr.right - wr.left,                   // Actual width size
			wr.bottom - wr.top,                   // Actual height size
			m_hWndPar,                            //
			NULL,                                 //
			NULL,                                 //
			NULL                                  //
		);

		// Create window failed or associate failed
		if (!m_hWnd)
		{
			this->DeleteGDIplus();
			return false;
		}
		SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);

		// Update size window after created
		RECT rect;
		if (GetClientRect(m_hWnd, &rect))
		{
			m_CurStatus.m_rect.width = rect.right - rect.left;
			m_CurStatus.m_rect.height = rect.bottom - rect.top;
		}
	}

	WinConsoleStyle CreateConsoleStyle()
	{

	}

public:
	virtual void* GetHandle()
	{
		return m_hWnd;
	}

	virtual bool Create(const TCHAR* strTitle, unsigned int nWidth, const int nHeight)
	{

	}
	virtual void SetWindowSize(const int nRow, const int nCol) = 0;
	virtual void SetWindowPosition(const int xPos, const int yPos) = 0;
	virtual void SetCellSize(const int nWidth, const int nHeight) = 0;
	virtual void ShowCursor(bool bShow) = 0;
	virtual void Show() = 0;
	virtual void Hide() = 0;
	virtual void SetFont(const TCHAR* strFont) = 0;
	virtual void SetTitle(const TCHAR* strTitle) = 0;
	virtual bool Closed() const = 0;
	virtual void Draw() = 0;

public:
	virtual void PollEvent()
	{
		// Check to see if any messages are waiting in the queue
		while (PeekMessage(&m_MSG, NULL, 0, 0, PM_REMOVE))
		{
			// Translate the message and dispatch it to WindowProc()
			TranslateMessage(&m_MSG);
			DispatchMessage(&m_MSG);
		}
	}

	virtual void WaitEvent()
	{
		// Handle on event case
		if (GetMessage(&m_MSG, NULL, 0, 0) > 0)
		{
			TranslateMessage(&m_MSG);
			DispatchMessage(&m_MSG);
		}
	}

protected:
	HWND	m_hWnd;
	MSG		m_MSG;

	WinConsoleStyle m_Style;
	WinConsoleState m_State;
};