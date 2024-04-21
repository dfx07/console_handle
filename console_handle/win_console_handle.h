#pragma once

#include <Windows.h>

#include "console_handle.h"
#include "console_view.h"

#define WIN_CONSOLE_CLASS _T("WIN_CONSOLE_CLASS")

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

public:
	WinConsoleHandle()
	{
		if (!s_bRegisterClass)
		{
			s_bRegisterClass = RegisterConsoleClass(GetModuleHandle(NULL));
		}
	}

	~WinConsoleHandle()
	{

	}

protected:

	bool RegisterConsoleClass(HINSTANCE hInst)
	{
		WNDCLASSEX wClass;
		ZeroMemory(&wClass, sizeof(WNDCLASSEX));
		wClass.cbClsExtra		= NULL;
		wClass.cbSize			= sizeof(WNDCLASSEX);
		wClass.cbWndExtra		= NULL;
		wClass.hbrBackground	= (HBRUSH)(COLOR_MENU);
		wClass.hCursor			= LoadCursor(NULL, IDC_ARROW);
		wClass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
		wClass.hIconSm			= NULL;
		wClass.hInstance		= hInst;
		wClass.lpfnWndProc		= (WNDPROC)ConsoleMainProc;
		wClass.lpszClassName	= WIN_CONSOLE_CLASS;
		wClass.lpszMenuName		= NULL;
		wClass.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

		return !!RegisterClassEx(&wClass);
	}

	HWND CreateConsoleHandle(const TCHAR* strWndClassName, const TCHAR* title, WinConsoleState& state, WinConsoleStyle& style)
	{
		// Adjust the size
		RECT wr = { 0, 0, (LONG)state.width, (LONG)state.height };
		AdjustWindowRect(&wr, style.m_dwStyle, FALSE);

		HWND hWnd = CreateWindowEx(style.m_dwStyleEx,//
			strWndClassName,                         //
			title,                                   //
			style.m_dwStyle,                         //
			state.xpos,                              //
			state.ypos,                              // Postion 
			wr.right - wr.left,                      // Actual width size
			wr.bottom - wr.top,                      // Actual height size
			m_hWndParent,                            //
			NULL,                                    //
			NULL,                                    //
			NULL                                     //
		);

		if (hWnd == NULL)
			return NULL;

		// Create window failed or associate failed
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)this);

		// Update size window after created
		RECT rect;
		if (GetClientRect(hWnd, &rect))
		{
			state.width = rect.right - rect.left;
			state.height = rect.bottom - rect.top;
		}

		return hWnd;
	}

	WinConsoleStyle CreateConsoleStyle()
	{
		WinConsoleStyle style;

		style.m_dwStyleEx = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;  // Window Extended Style
		style.m_dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_EX_TRANSPARENT;  // Windows Style

		 // Are We Still In Fullscreen Mode?
		if (m_bFullScreen)
		{
			// Window Extended Style
			style.m_dwStyleEx = style.m_dwStyleEx & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE
				| WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);

			// Windows Style
			style.m_dwStyle = style.m_dwStyle & ~(WS_CAPTION | WS_THICKFRAME);
		}

		if (m_bVisible == false)
		{
			// dwMyFlags ^= dwSomeFlag; remove flag 
			style.m_dwStyle &= ~WS_VISIBLE;
		}

		return style;
	}

	bool CreateConsoleGraphics()
	{

	}

protected:
	bool SetMouseEvent(ConsoleMouseButton& btn, int state, ConsoleMousePos& pos)
	{
		m_MouseEvent.m_MouseButton = btn;
		m_MouseEvent.m_MouseState = state;
		m_MouseEvent.m_MousePos = pos;
	}

	bool SetKeyboardEvent(int nKeyCode, int nState )
	{
		m_KeyboardEvent.m_nKey = nKeyCode;
		m_KeyboardEvent.m_nState = nState;
	}

protected:

	static LRESULT CALLBACK ConsoleMainProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		WinConsoleHandle* console = (WinConsoleHandle*)(GetWindowLongPtr(hWnd, GWLP_USERDATA));

		if (!console)
			return DefWindowProc(hWnd, message, wParam, lParam);

		switch (message)
		{
			case WM_CLOSE:
			{
				console->m_bClosed = true;
				break;
			}
			case WM_DESTROY:
			{
				console->m_bClosed = true;
				break;
			}
			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
			{
				//win->SetKeyboardStatus((int)wParam, true);
				//win->OnKeyBoard(win);
				break;
			}
			case WM_KEYUP:
			case WM_SYSKEYUP:
			{
				//win->SetKeyboardStatus((int)wParam, false);
				//win->OnKeyBoard(win);
				break;
			}
			case WM_LBUTTONUP:
			{
				//win->SetMouseButtonStatus(VK_LBUTTON, false);
				//win->OnMouseButton(win, GLMouse::LeftButton, GL_RELEASE);
				break;
			}
			case WM_RBUTTONUP:
			{
				//win->SetMouseButtonStatus(VK_RBUTTON, false);
				//win->OnMouseButton(win, GLMouse::RightButton, GL_RELEASE);
				break;
			}
			case WM_LBUTTONDOWN:
			{
				//win->SetMouseButtonStatus(VK_LBUTTON, true);
				//win->OnMouseButton(win, GLMouse::LeftButton, GL_PRESSED);


				break;
			}
			case WM_RBUTTONDOWN:
			{
				//win->SetMouseButtonStatus(VK_RBUTTON, true);
				//win->OnMouseButton(win, GLMouse::RightButton, GL_PRESSED);
				break;
			}
			case WM_MOUSEMOVE:
			{
				//for (int i = 0; i < win->m_ChildWindowList.size(); i++)
				//{
				//	win->m_ChildWindowList[i]->Send_Message(static_cast<UINT>(MOVE_WIN),
				//		static_cast<WPARAM>(PROCESS_MSG), 0);
				//}

				//win->OnMouseMove(win);
				break;
			}
			case WM_SIZE: //Check if the window has been resized
			{
				//win->m_CurStatus.m_rect.width = LOWORD(lParam); // width
				//win->m_CurStatus.m_rect.height = HIWORD(lParam); // height

				//win->m_text_render.UpdateView(win->m_CurStatus.m_rect.width, win->m_CurStatus.m_rect.height);

				//win->UpdateTitle();

				//// Cannot use OpenGL context in this tunnel
				//win->OnResize(win);

				//// Refresh screen when resize window in case one thread
				//if (win->m_iDrawMode == 0)
				//{
				//	win->OnDraw();
				//}

				break;
			}
			case WM_SIZING:
			{
				break;
			}
			case WM_MOUSEWHEEL:
			{
				//win->m_zDeltaScroll = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
				//win->OnMouseScroll(win);
				break;
			}
			case WM_PAINT:
			{
				//console->OnPaint(win);
				break;
			}
			case WM_ERASEBKGND:
			{

				return TRUE;
			}
			default:
			{
				break;
			}
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

public:
	virtual void* GetHandle()
	{
		return m_hWnd;
	}

	virtual bool Create(const TCHAR* strTitle, int xpos, int ypos, unsigned int nWidth, const int nHeight)
	{
		m_State.xpos = xpos;
		m_State.ypos = ypos;
		m_State.width = nWidth;
		m_State.height = nHeight;

		m_Style = CreateConsoleStyle();

		m_hWnd = CreateConsoleHandle(WIN_CONSOLE_CLASS, strTitle, m_State, m_Style);

		return !!m_hWnd;
	}

	virtual void SetWindowSize(const int nRow, const int nCol)
	{

	}

	virtual void SetWindowPosition(const int xPos, const int yPos)
	{

	}

	virtual void SetCellSize(const int nWidth, const int nHeight)
	{

	}

	virtual void ShowCursor(bool bShow)
	{

	}

	virtual void Show()
	{
		::ShowWindow(m_hWnd, SW_SHOW);
	}

	virtual void Hide()
	{
		::ShowWindow(m_hWnd, SW_HIDE);
	}

	virtual void SetFont(const TCHAR* strFont) = 0;
	virtual void SetTitle(const TCHAR* strTitle) = 0;
	virtual bool Closed() const
	{
		return m_bClosed;
	}

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
	static bool s_bRegisterClass;

	HWND	m_hWnd;
	HWND	m_hWndParent;
	MSG		m_MSG;

	WinConsoleStyle m_Style;
	WinConsoleState m_State;
};

bool WinConsoleHandle::s_bRegisterClass = false;