#include <iostream>
#include "win_console_handle.h"

void Draw(ConsoleHandle* handle, ConsoleView* pView)
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
	if (pMouse->m_MouseState == ConsoleMouseState::MOUSE_DOWN_STATE)
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
}

int main()
{
	WinConsoleHandle win;
	win.SetMouseEventCallback(MouseCallback);
	win.SetWindowSize

	if (!win.Create(_T("console handle"), 680, 480))
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