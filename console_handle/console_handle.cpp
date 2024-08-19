#include <iostream>
#include "win_console_handle.h"
#include "xgeo.h"
#include "xutil.h"

enum EventState
{
	Free		= 0x0000,
	InputBrick	= 0x0001,
	SelectStart	= 0x0002,
	SelectEnd	= 0x0004
};

EventState state = EventState::Free;
ConsolePoint ptCurCursor;
ConsolePoint ptStart{-1, -1};
ConsolePoint ptEnd{ -1, -1 };
std::vector<ConsolePoint> brick;

ConsoleString GetStateString(EventState state)
{
	switch (state)
	{
	case Free:
		return _T("FREE");
		break;
	case InputBrick:
		return _T("InputBrick");
		break;
	case SelectStart:
		return _T("SelectStart");
		break;
	case SelectEnd:
		return _T("SelectEnd");
	default:
		break;
	}
	return _T("None");
}

ConsoleString strState;


void DrawCallback(ConsoleHandle* handle, ConsoleGraphics* pGraphic)
{
	ConsoleColor col{ 255, 0, 0 };
	ConsoleColor colstart{ 255, 0, 0 };
	ConsoleColor colEnd{ 0, 255, 0 };
	ConsoleColor colbrick{ 255, 255, 0 };

	strState = GetStateString(state);

	pGraphic->SetActiveFont({_T("Arial"), 6});
	pGraphic->SetTextCell(0, 1, strState.c_str());

	pGraphic->SetBorderColor((int)ptCurCursor.x, (int)ptCurCursor.y, col);

	for (int i = 0; i < brick.size(); i++)
	{
		pGraphic->SetColorCell((int)brick[i].x, (int)brick[i].y, colbrick);
	}

	pGraphic->SetColorCell((int)ptStart.x, (int)ptStart.y, colstart);
	pGraphic->SetColorCell((int)ptEnd.x, (int)ptEnd.y, colEnd);
}

void KeyboardCallback(ConsoleHandle* handle, KeyBoardEventInfo* pKeyboard)
{
	if (pKeyboard->m_eState == ConsoleKeyboardState::KEYBOARD_DOWN_STATE)
	{
		if (pKeyboard->m_eKey == ConsoleKeyboard::KeyI)
		{
			state = EventState::InputBrick;
		}
		else if (pKeyboard->m_eKey == ConsoleKeyboard::KeyS)
		{
			state = EventState::SelectStart;
		}
		else if (pKeyboard->m_eKey == ConsoleKeyboard::KeyE)
		{
			state = EventState::SelectEnd;
		}
		else if (pKeyboard->m_eKey == ConsoleKeyboard::Escapex)
		{
			state = EventState::Free;
		}
	}
}

void MouseCallback(ConsoleHandle* handle, MouseEventInfo* pMouse)
{
	if (pMouse->m_MouseState == ConsoleMouseState::MOUSE_MOVE_STATE)
	{
		ptCurCursor = { pMouse->m_MousePos.x, pMouse->m_MousePos.y };

		if (state == EventState::InputBrick)
		{
			brick.push_back(ptCurCursor);
		}
	}
	else if (pMouse->m_MouseState == ConsoleMouseState::MOUSE_DOWN_STATE)
	{
		if (state == SelectStart)
		{
			ptStart = { pMouse->m_MousePos.x, pMouse->m_MousePos.y };
		}
		else if (state == SelectEnd)
		{
			ptEnd = { pMouse->m_MousePos.x, pMouse->m_MousePos.y };
		}
	}
}

int main()
{
	brick.reserve(500);
	strState.reserve(30);

	WinConsoleHandle win;
	win.SetMouseEventCallback(MouseCallback);
	win.SetKeyboardEventCallback(KeyboardCallback);
	win.SetDrawCallback(DrawCallback);
	win.SetWindowSize(100, 100);
	//win.SetCellSize(20, 20);

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
