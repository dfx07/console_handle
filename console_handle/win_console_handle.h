#pragma once

#include <Windows.h>
#include <tuple>

#include "console_handle.h"
#include "console_view.h"
#include "win_console_type.h"
#include "win_console_font.h"
#include "gl_render.h"


#define WM_UPDATE_DRAW		WM_USER + 0x0001

class WinConsoleHandle : public ConsoleHandle, public ConsoleHandleEvent
{
	static constexpr const TCHAR* WIN_CONSOLE_CLASS = _T("WIN_CONSOLE_CLASS");

	struct WinConsoleStyle
	{
		DWORD m_dwStyle;
		DWORD m_dwStyleEx;
	};

public:
	WinConsoleHandle()
	{
		if (!s_bRegisterClass)
		{
			s_bRegisterClass = RegisterConsoleClass(GetModuleHandle(NULL));
		}

		if (!s_bRegisterClass)
			throw _T("Register class failed!");
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

	HWND CreateConsoleHandle(const TCHAR* strWndClassName, const TCHAR* title, ConsoleHandleState& state, WinConsoleStyle& style)
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

	bool CreateConsoleGraphics(ConsoleDeviceEngine eEngine)
	{
		DeviceContextConfig config;

		auto pGraphics = static_cast<ConsoleGraphics*>(m_pView->GetGraphics());

		pGraphics->SetModelData(m_pModelData.get());

		if (!pGraphics->UpdateDrawBoardData())
		{
			throw _T("UpdateDrawBoardData failed !");
			return false;
		}

		if (ConsoleDeviceEngine::OPENGL == eEngine)
		{
			auto pOpenGLDevice = std::make_shared<OpenGLConsoleDevice>();

			if (pOpenGLDevice->ConsoleDeviceContext(this, config))
			{
				m_pDevice = pOpenGLDevice;
			}
		}

		if (m_pDevice)
		{
			m_pDevice->GetDeviceControl()->SetFontManager(m_pModelData->GetFontManager());

			DeviceContextPtr pContext = m_pDevice->GetContext();

			HDC* pHDC = reinterpret_cast<HDC*>(pContext->Render());

			ConsoleString fontDefaultName = _T("Consolas");

			auto pTempFont = std::make_shared<WinConsoleFont>();
			if (pTempFont->Load(fontDefaultName, 10, ConsoleFontType::Normal))
			{
				auto fontSize = WinConsoleFont::GetFontSizeFrom(*pHDC, *static_cast<HFONT*>(pTempFont->GetHandle()),
					(unsigned int)m_pView->GetHeightCell());

				auto pDefaultFont = std::make_shared<WinConsoleFont>();
				if (pDefaultFont->Load(fontDefaultName, fontSize, ConsoleFontType::Normal))
				{
					m_pModelData->SetDefaultFont(pDefaultFont);
				}
			}
		}

		return m_pDevice != NULL;
	}

protected:
	virtual void OnMouseEvent() { ON_FUNCTION(m_funOnMouseEvent, this, &m_MouseEvent) }
	virtual void OnKeyBoardEvent() { ON_FUNCTION(m_funOnKeyboardEvent, this, &m_KeyboardEvent) }
	virtual void OnResizeEvent() { ON_FUNCTION(m_funOnResizeEvent, this) }
	virtual void OnDraw() { ON_FUNCTION(m_funOnDraw, this, static_cast<ConsoleGraphics*>(m_pView->GetGraphics())) }

protected:
	virtual bool CreateBoardView(const unsigned int nWidth, const unsigned int nHeight)
	{
		auto pView = std::make_shared<ConsoleView>();

		pView->SetViewSize(nWidth, nHeight);
		pView->SetModelData(m_pModelData.get());

		m_pView = pView;

		return true;
	}

	virtual bool CreateBoardModel(const int nRow, const int nCol)
	{
		m_pModelData = std::make_shared<ConsoleBoardModelData>();

		m_pModelData->SetSize(nRow, nCol);
		m_pModelData->CreateBoardData();



		return true;
	}

protected:
	bool SetMouseEvent(ConsoleMouseButton btn, int state, ConsoleMousePos& pos, bool bcall_event = true)
	{
		m_MouseEvent.m_MouseButton = btn;
		m_MouseEvent.m_MouseState = state;
		m_MouseEvent.m_MousePos = pos;

		if (bcall_event)
		{
			OnMouseEvent();
		}

		m_oldPos = pos;

		return true;
	}

	bool SetKeyboardEvent(ConsoleKeyboard eKey, ConsoleKeyboardState eState, bool bcall_event = true)
	{
		m_KeyboardEvent.m_eKey = eKey;
		m_KeyboardEvent.m_eState = eState;

		if (bcall_event)
		{
			OnKeyBoardEvent();
		}

		return true;
	}

	ConsoleMousePos GetConsolePosFromClient(int xpos, int ypos)
	{
		auto idx = m_pView->GetCell(xpos, ypos);

		return ConsoleMousePos{ idx.m_iY, idx.m_iX };
	}

	bool IsValidConsolePos(ConsoleMousePos& pos)
	{
		return m_pView->IsValidCellIndex(ConsoleCellIndex{ pos.x, pos.y });
	}

	bool IsSameOldCurPos(ConsoleMousePos& pos)
	{
		return (m_oldPos.x == pos.x && m_oldPos.y == pos.y);
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
				console->SetKeyboardEvent(static_cast<ConsoleKeyboard>(wParam), ConsoleKeyboardState::KEYBOARD_DOWN_STATE);
				break;
			}
			case WM_KEYUP:
			case WM_SYSKEYUP:
			{
				console->SetKeyboardEvent(static_cast<ConsoleKeyboard>(wParam), ConsoleKeyboardState::KEYBOARD_UP_STATE);
				break;
			}
			case WM_LBUTTONUP:
			{
				WORD ypos = HIWORD(lParam);
				WORD xpos = LOWORD(lParam);

				auto cpos = console->GetConsolePosFromClient(xpos, ypos);
				if (console->IsValidConsolePos(cpos))
				{
					console->SetMouseEvent(ConsoleMouseButton::MOUSE_BUTTON_LEFT, ConsoleMouseState::MOUSE_UP_STATE, cpos);
				}

				break;
			}
			case WM_RBUTTONUP:
			{
				WORD ypos = HIWORD(lParam);
				WORD xpos = LOWORD(lParam);

				auto cpos = console->GetConsolePosFromClient(xpos, ypos);
				if (console->IsValidConsolePos(cpos))
				{
					console->SetMouseEvent(ConsoleMouseButton::MOUSE_BUTTON_RIGHT, ConsoleMouseState::MOUSE_UP_STATE, cpos);
				}
				break;
			}
			case WM_LBUTTONDOWN:
			{
				WORD ypos = HIWORD(lParam);
				WORD xpos = LOWORD(lParam);

				auto cpos = console->GetConsolePosFromClient(xpos, ypos);
				if (console->IsValidConsolePos(cpos))
				{
					console->SetMouseEvent(ConsoleMouseButton::MOUSE_BUTTON_LEFT, ConsoleMouseState::MOUSE_DOWN_STATE, cpos);
				}

				break;
			}
			case WM_RBUTTONDOWN:
			{
				WORD ypos = HIWORD(lParam);
				WORD xpos = LOWORD(lParam);

				auto cpos = console->GetConsolePosFromClient(xpos, ypos);
				if (console->IsValidConsolePos(cpos))
				{
					console->SetMouseEvent(ConsoleMouseButton::MOUSE_BUTTON_RIGHT, ConsoleMouseState::MOUSE_DOWN_STATE, cpos);
				}
				break;
			}
			case WM_MOUSEMOVE:
			{
				if (!console->m_bTracking)
				{
					TRACKMOUSEEVENT me{};
					me.cbSize = sizeof(TRACKMOUSEEVENT);
					me.dwFlags = TME_HOVER | TME_LEAVE;
					me.hwndTrack = hWnd;
					me.dwHoverTime = HOVER_DEFAULT;
					console->m_bTracking = TrackMouseEvent(&me);
				}

				WORD ypos = HIWORD(lParam);
				WORD xpos = LOWORD(lParam);

				auto cpos = console->GetConsolePosFromClient(xpos, ypos);
				
				if (console->IsValidConsolePos(cpos) && !console->IsSameOldCurPos(cpos))
				{
					std::cout << cpos.x << ":" << cpos.y << std::endl;
					console->SetMouseEvent(ConsoleMouseButton::MOUSE_BUTTON_NONE, ConsoleMouseState::MOUSE_MOVE_STATE, cpos);
				}
				break;
			}
			case WM_MOUSELEAVE:
			{
				WORD ypos = HIWORD(lParam);
				WORD xpos = LOWORD(lParam);

				auto cpos = console->GetConsolePosFromClient(xpos, ypos);

				console->SetMouseEvent(ConsoleMouseButton::MOUSE_BUTTON_NONE, ConsoleMouseState::MOUSE_LEAVE, cpos);
				console->m_bTracking = FALSE;
				break;
			}
			case WM_SIZE:
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
			case WM_UPDATE_DRAW:
			{
				console->Draw();
				break;
			}
			default:
			{
				break;
			}
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

public:
	virtual void* GetHandle() noexcept
	{
		return m_hWnd;
	}

	virtual void* GetHandleParent() noexcept
	{
		return m_hWndParent;
	}

	virtual bool Create(const TCHAR* strTitle, int xpos, int ypos, unsigned int nWidth, const int nHeight)
	{
		bool bCreateDone = false;
		m_State.xpos = xpos;
		m_State.ypos = ypos;
		m_State.width = nWidth;
		m_State.height = nHeight;

		int nerr = 0;

		m_Style = CreateConsoleStyle();

		while (true)
		{
			m_hWnd = CreateConsoleHandle(WIN_CONSOLE_CLASS, strTitle, m_State, m_Style);

			if (m_hWnd)
			{
				if (!CreateBoardModel(m_nRows, m_nCols))
				{
					nerr = -1;
					throw _T("CreateBoardModel failed !");
					break;
				}

				if(!CreateBoardView(m_State.width, m_State.height))
				{
					nerr = -1;
					throw _T("CreateBoardView failed !");
					break;
				}

				if (!CreateConsoleGraphics(ConsoleDeviceEngine::OPENGL))
				{
					nerr = -1;
					throw _T("CreateConsoleGraphics failed !");
					break;
				}
			}

			break;
		}

		return !(!!nerr);
	}

	virtual void SetWindowSize(const int nRow, const int nCol)
	{
		m_nRows = nRow;
		m_nCols = nCol;
	}

	virtual void SetWindowPosition(const int xPos, const int yPos)
	{
		HWND hWnd = static_cast<HWND>(GetHandle());

		if (!hWnd)
			return;

		::SetWindowPos(hWnd, NULL, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}

	virtual void SetWindowCenter()
	{
		HWND hWnd = static_cast<HWND>(GetHandle());
		HWND hWndPar = static_cast<HWND>(GetHandleParent());

		if (!hWnd)
			return;

		using _tPWindowPos = std::tuple<int, int, int, int>;

		RECT rectWindow, rectParent;
		_tPWindowPos tpWinPos;

		auto funCalcPosCenterWindow = [](RECT& rcWin, RECT& rcWinPar) -> _tPWindowPos
		{
			int nWidth = rcWin.right - rcWin.left;
			int nHeight = rcWin.bottom - rcWin.top;

			int nX = ((rcWinPar.right - rcWinPar.left) - nWidth) / 2 + rcWinPar.left;
			int nY = ((rcWinPar.bottom - rcWinPar.top) - nHeight) / 2 + rcWinPar.top;

			int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
			int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

			// make sure that the dialog box never moves outside of the screen
			if (nX < 0) nX = 0;
			if (nY < 0) nY = 0;
			if (nX + nWidth > nScreenWidth) nX = nScreenWidth - nWidth;
			if (nY + nHeight > nScreenHeight) nY = nScreenHeight - nHeight;

			return std::make_tuple(nX, nY, nWidth, nHeight);
		};

		GetWindowRect(hWnd, &rectWindow);

		// make the window relative to its parent
		if (hWndPar != NULL)
		{
			GetWindowRect(hWndPar, &rectParent);
		}
		else
		{
			rectParent.top = 0;
			rectParent.left = 0;
			rectParent.right = GetSystemMetrics(SM_CXSCREEN);
			rectParent.bottom = GetSystemMetrics(SM_CYSCREEN);
		}

		tpWinPos = funCalcPosCenterWindow(rectWindow, rectParent);
		::MoveWindow(hWnd, std::get<0>(tpWinPos), std::get<1>(tpWinPos), std::get<2>(tpWinPos), std::get<3>(tpWinPos), FALSE);
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

	virtual void SetFont(const TCHAR* strFont)
	{

	}

	virtual void SetTitle(const TCHAR* strTitle)
	{
		memcpy(m_strTitle, strTitle, strlen((char*)strTitle) * 2);

		if (m_hWnd)
		{
			::SetWindowText(m_hWnd, m_strTitle);
		}
	}

	virtual bool Closed() const
	{
		return m_bClosed;
	}

	virtual void Draw()
	{
		auto pGraphic = static_cast<ConsoleGraphics*>(m_pView->GetGraphics());

		if (m_pDevice->Begin(m_pView.get()))
		{
			if (m_funOnDraw)
				m_funOnDraw(this, pGraphic);

			m_pDevice->Clear();

			m_pDevice->Update();

			m_pDevice->Draw();

			m_pDevice->End();
		}
		else
		{
			OutputDebugString(_T("Device begin failed !"));
		}

		pGraphic->Clear();
	}

	virtual void Update()
	{
		::SendMessage( m_hWnd, WM_UPDATE_DRAW, 0L, 0L);
	}

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

	HWND				m_hWnd{ NULL };
	HWND				m_hWndParent{ NULL };
	MSG					m_MSG;

	WinConsoleStyle		m_Style;

	ConsoleHandleState	m_State;
	ConsoleMousePos		m_oldPos;

	BOOL				m_bTracking{ FALSE };
};

bool WinConsoleHandle::s_bRegisterClass = false;