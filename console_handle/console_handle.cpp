#include <iostream>
#include "win_console_handle.h"

void Draw(ConsoleHandle* handle, ConsoleGraphics* pGraphic)
{

}

void KeyboardCallback(ConsoleHandle* handle, KeyBoardEventInfo* pKeyboard)
{

}

void MouseCallback(ConsoleHandle* handle, MouseEventInfo* pMouse)
{
	if (pMouse->m_MouseState == ConsoleMouseState::MOUSE_MOVE_STATE)
	{
		OutputDebugString(_T("[move] \n"));
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
	win.SetWindowSize(50, 50);
	win.SetCellSize(20, 20);

	if (!win.Create(_T("console handle"), 100, 100, 680, 480))
	{
		exit(-1);
	}

	win.Show();

	while (!win.Closed())
	{
		win.Draw();

		win.WaitEvent();
	}
}