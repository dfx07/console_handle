#include <iostream>
#include "win_console_handle.h"
#include <alg/xastar.h>
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
		return _T("InputBrick");
		break;
	case SelectStart:
		return _T("SelectStart");
		break;
	case SelectEnd:
		return _T("SelectEnd");
	case Find:
		return _T("Find");
	default:
		break;
	}
	return _T("None");
}

ConsoleString strState;


void Perform(AstarCellPriorityQueue& _priority, _stAStarGridCellPF* pCellcur)
{
	std::unique_lock<std::mutex> lock(mtx, std::defer_lock);

	lock.lock();

	priority.clear();

	AstarCellPriorityQueue temp = _priority;

	while (!temp.empty()) {
		priority.push_back({ temp.top()->pGrid->stIdx.nX, temp.top()->pGrid->stIdx.nY });
		temp.pop();
	}

	ptCur = { pCellcur->pGrid->stIdx.nX, pCellcur->pGrid->stIdx.nY };

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
	std::vector<stGridCellPF> vecData; vecData.resize(brick.size());

	for (int i = 0 ; i < brick.size() ; i++)
	{
		vecData[i].stIdx = { (int)brick[i].x ,(int)brick[i].y };
		vecData[i].stCellData.fWeight = 1.f;
	}

	pGridPF->BuildFrom(vecData, nRows, nCols);

	AStar* pAstar = new AStar();
	pAstar->m_pFunPerform = &Perform;

	PathFinder pathFinder;

	pathFinder.SetOptionAllowCross(true);
	pathFinder.SetOptionDontCrossCorners(true);

	pathFinder.Prepar(pGridPF, pAstar);

	auto vec = pathFinder.Search({ (int)ptStart.x, (int)ptStart.y }, { (int)ptEnd.x, (int)ptEnd.y });

	path.clear();
	path.reserve(vec.size());

	for (int i = 0; i < vec.size(); i++)
	{
		path.push_back({ vec[i]->stIdx.nX, vec[i]->stIdx.nY });
	}

	delete pGridPF;
	delete pAstar;

	double tm = timer.elapsed_to_mili();

	std::cout << "Time : " << tm << " ms" << std::endl;

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

	pGraphic->SetActiveFont({_T("Arial"), 6});
	pGraphic->SetTextCell(0, 1, strState.c_str());

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
	win.SetWindowSize(100, 100);

	pHandle = &win;

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
