#include <iostream>
#include "win_console_handle.h"
#include "xgeo.h"
#include "xutil.h"

void DrawCallback(ConsoleHandle* handle, ConsoleGraphics* pGraphic)
{
	//std::cout << "draw \n" << std::endl;

	ConsoleColor col{ 255, 0, 0 };

	pGraphic->SetTextCell(1, 2, _T("Ngo vawn thuong"));
	//pGraphic->SetBorderColor(10, 10, col);
}

void KeyboardCallback(ConsoleHandle* handle, KeyBoardEventInfo* pKeyboard)
{
	if (pKeyboard->m_eState == ConsoleKeyboardState::KEYBOARD_DOWN_STATE)
	{
		OutputDebugString(_T("[key] key down\n"));
	}
	else if (pKeyboard->m_eState == ConsoleKeyboardState::KEYBOARD_UP_STATE)
	{
		OutputDebugString(_T("[key] key up\n"));
	}
}

void MouseCallback(ConsoleHandle* handle, MouseEventInfo* pMouse)
{
	if (pMouse->m_MouseState == ConsoleMouseState::MOUSE_MOVE_STATE)
	{
		ConsoleColor col{ 255, 0, 0 };

		ConsoleGraphics* pGraphic = handle->GetView()->GetGraphics();

		pGraphic->SetBorderColor(pMouse->m_MousePos.x, pMouse->m_MousePos.y, col);

	}
	else if (pMouse->m_MouseState == ConsoleMouseState::MOUSE_DOWN_STATE)
	{
		if (pMouse->m_MouseButton == ConsoleMouseButton::MOUSE_BUTTON_LEFT)
		{
			OutputDebugString(_T("[left] mouse down \n"));
		}
		else if (pMouse->m_MouseButton == ConsoleMouseButton::MOUSE_BUTTON_RIGHT)
		{
			OutputDebugString(_T("[right] mouse down \n"));
		}
	}
	else if (pMouse->m_MouseState == ConsoleMouseState::MOUSE_UP_STATE)
	{
		if (pMouse->m_MouseButton == ConsoleMouseButton::MOUSE_BUTTON_LEFT)
		{
			OutputDebugString(_T("[left] mouse up \n"));
		}
		else if (pMouse->m_MouseButton == ConsoleMouseButton::MOUSE_BUTTON_RIGHT)
		{
			OutputDebugString(_T("[right] mouse up \n"));
		}
	}
}

int main()
{
	WinConsoleHandle win;
	win.SetMouseEventCallback(MouseCallback);
	win.SetKeyboardEventCallback(KeyboardCallback);
	win.SetDrawCallback(DrawCallback);
	win.SetWindowSize(100, 100);
	win.SetCellSize(20, 20);

	if (!win.Create(_T("console handle"), 100, 100, 680, 680))
	{
		exit(-1);
	}

	win.SetWindowCenter();
	win.Show();

	while (!win.Closed())
	{
		win.Draw();

		win.WaitEvent();
	}
}
