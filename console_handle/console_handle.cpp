#include <iostream>
#include "win_console_handle.h"
#include <alg/xthetastar.h>
#include <thread>
#include <mutex>
#include "com/xtimer.h"

enum EventState
{
	Free		= 0x0000,
	InputBrick	= 0x0001,
	SelectStart	= 0x0002,
	SelectEnd	= 0x0004,
	Find		= 0x0008
};

EventState state = EventState::Free;
ConsolePoint ptCurCursor;
ConsolePoint ptStart{20, 20};
ConsolePoint ptEnd{ 23, 23 };

ConsoleHandle* pHandle;

std::thread t1;
std::mutex mtx;

std::vector<ConsolePoint> brick;
std::vector<ConsolePoint> path;

std::vector<ConsolePoint> priority;
ConsolePoint ptCur;

ConsoleString GetStateString(EventState state)
{
	switch (state)
	{
	case Free:
		return _T("FREE");
		break;
	case InputBrick:
		return _T("Input wall");
		break;
	case SelectStart:
		return _T("Select Start");
		break;
	case SelectEnd:
		return _T("Select End");
	case Find:
		return _T("Find");
	default:
		break;
	}
	return _T("None");
}

ConsoleString strState;


void Perform(std::set<stCellPF*>& _priority, stCellPF* pCellcur)
{
	std::unique_lock<std::mutex> lock(mtx, std::defer_lock);

	lock.lock();

	priority.clear();

	for (auto itr = _priority.begin(); itr != _priority.end(); itr++)
	{
		priority.push_back({ (*itr)->stIdx.nX,(*itr)->stIdx.nY });
	}

	ptCur = { pCellcur->stIdx.nX, pCellcur->stIdx.nY };

	lock.unlock();
	pHandle->Update();
}



void FindPathFinding(ConsoleHandle* handle)
{
	Timer timer;

	timer.reset();

	GridPF* pGridPF = new GridPF();
	unsigned int nRows = handle->GetRows();
	unsigned int nCols = handle->GetColumns();
	std::vector<stCellPF> vecData; vecData.resize(brick.size());

	for (int i = 0 ; i < brick.size() ; i++)
	{
		vecData[i].stIdx = { (int)brick[i].x ,(int)brick[i].y };
		vecData[i].stData.fWeight = 1.f;
	}

	pGridPF->BuildFrom(vecData, nRows, nCols);

	ThetaStar* pTheta = new ThetaStar();
	pTheta->SetFuncPerform(&Perform);

	PathFinder pathFinder;

	pathFinder.SetOptionAllowCross(true);
	pathFinder.SetOptionDontCrossCorners(true);

	pathFinder.Prepar(pGridPF, pTheta);

	auto vec = pathFinder.Search({ (int)ptStart.x, (int)ptStart.y }, { (int)ptEnd.x, (int)ptEnd.y });

	path.clear();
	path.reserve(vec.size());

	for (int i = 0; i < vec.size(); i++)
	{
		path.push_back({ vec[i]->stIdx.nX, vec[i]->stIdx.nY });
	}

	delete pGridPF;
	delete pTheta;

	double tm = timer.elapsed_to_mili();

	std::cout << "Time : " << tm << " ms" << std::endl;

	//priority.clear();

	//int dx = ptEnd.x - ptStart.x;
	//int dy = ptEnd.y - ptStart.y;
	//int D = 2 * dy - dx;
	//int y = ptStart.y;

	//for (int x = ptStart.x; x <= ptEnd.x; x++)
	//{

	//	priority.push_back({ x, y });

	//	if (D > 0)
	//	{
	//		y = y + 1;
	//		D = D - 2 * dx;
	//	}
	//	D = D + 2 * dy;
	//}

	//int y = ptStart.y;

	//int dx = ptEnd.x - ptStart.x;
	//int dy = ptEnd.y - ptStart.y;
	//int slope = 2 * dy;
	//int error = -dx;
	//int errorInc = -2 * dx;

	//for (int x = ptStart.x; x <= ptEnd.x; ++x)
	//{
	//	priority.push_back({ x, y });
	//	error += slope;

	//	if (error >= 0)
	//	{
	//		y++;
	//		error += errorInc;
	//	}
	//}


	handle->Update();
}

void DrawCallback(ConsoleHandle* handle, ConsoleGraphics* pGraphic)
{
	std::unique_lock<std::mutex> lock(mtx);

	ConsoleColor col{ 255, 0, 0 };
	ConsoleColor colstart{ 255, 0, 0 };
	ConsoleColor colEnd{ 0, 255, 0 };
	ConsoleColor colbrick{ 255, 255, 0 };
	ConsoleColor colpath{ 255, 128, 128 };
	ConsoleColor colpri{ 0, 0, 255 };
	ConsoleColor colcur{ 0, 255, 0 };

	strState = GetStateString(state);

	pGraphic->SetActiveFont({_T("Arial"), 7});
	pGraphic->SetTextCell(0, 1, _T("Press S  : set start point"));
	pGraphic->SetTextCell(0, 2, _T("Press E  : set end point"));
	pGraphic->SetTextCell(0, 3, _T("Press I  : input wall"));
	pGraphic->SetTextCell(0, 4, _T("Press F2 : find path"));

	pGraphic->SetTextCell(0, 6, strState.c_str(), colEnd);

	pGraphic->SetBorderColor((int)ptCurCursor.x, (int)ptCurCursor.y, col);

	for (int i = 0; i < brick.size(); i++)
	{
		pGraphic->SetColorCell((int)brick[i].x, (int)brick[i].y, colbrick);
	}

	for (int i = 0; i < path.size(); i++)
	{
		pGraphic->SetColorCell((int)path[i].x, (int)path[i].y, colpath);
	}

	for (int i = 0; i < priority.size(); i++)
	{
		pGraphic->SetColorCell((int)priority[i].x, (int)priority[i].y, colpri);
	}

	pGraphic->SetBorderColor((int)ptCur.x, (int)ptCur.y, colcur);

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
		else if (pKeyboard->m_eKey == ConsoleKeyboard::F2)
		{
			state = EventState::Find;
		}
		else if (pKeyboard->m_eKey == ConsoleKeyboard::Delete)
		{
			path.clear();
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
		else if (state == Find)
		{
			t1 = std::thread(FindPathFinding, handle);
			if (t1.joinable())
			{
				t1.detach();
			}
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
	win.SetWindowSize(80, 80);

	pHandle = &win;

	if (!win.Create(_T("Findpathing : A star"), 100, 100, 680, 680))
		exit(-1);

	win.SetWindowCenter();
	win.Show();

	while (!win.Closed())
	{
		win.Draw();

		win.WaitEvent();
	}
}
