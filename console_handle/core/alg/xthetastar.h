////////////////////////////////////////////////////////////////////////////////////
/*!*********************************************************************************
*         Copyright (C) 2023-2024 thuong.nv <thuong.nv.mta@gmail.com>               
*                   MIT software Licencs, see the accompanying                      
************************************************************************************
* @brief : theta-star pathfinding
* @file  : xthetastar.h
* @create: Sep 15, 2024
* @note  : For conditions of distribution and use, see copyright notice in readme.txt
***********************************************************************************/
#ifndef XTHETASTAR_H
#define XTHETASTAR_H

#include "xastar.h"
#include "xpathfinder.h"

#define SGN(_x) ((_x) < 0 ? -1 : ((_x) > 0 ? 1 : 0))

class ThetaStar : public AStar
{
	virtual bool IsMoveCell2(_stAStarGridCellPF* _pCell)
	{
		bool bMove = AStar::IsMoveCell(_pCell);

		if (bMove)
			return IsMoveArround(_pCell, WayDirectionMode::Four);

		return bMove;
	}

	/*
	* Bresenham’s Line Generation
	*/
	virtual bool IsDrawLine(_stAStarGridCellPF* pS, _stAStarGridCellPF* pE)
	{
		int x0 = pS->pGrid->stIdx.nX;
		int x1 = pE->pGrid->stIdx.nX;
		int y0 = pS->pGrid->stIdx.nY;
		int y1 = pE->pGrid->stIdx.nY;

		int dx = x1 - x0;
		int dy = y1 - y0;
		int incX = SGN(dx);
		int incY = SGN(dy);
		dx = abs(dx);
		dy = abs(dy);

		auto funIsMoveable = [this](int x, int y)
		{
			_stAStarGridCellPF* pCellCur = GetAStarGridCell(x, y);
			return IsMoveCell(pCellCur);
		};

		if (dy == 0)
		{
			// horizontal line
			for (int x = x0; x != x1 + incX; x += incX)
			{
				if (!funIsMoveable(x, y0))
					return false;
			}
		}
		else if (dx == 0)
		{
			// vertical line
			for (int y = y0; y != y1 + incY; y += incY)
			{
				if (!funIsMoveable(x0, y))
					return false;
			}
		}
		else if (dx >= dy)
		{
			// more horizontal than vertical
			int slope = 2 * dy;
			int error = -dx;
			int errorInc = -2 * dx;
			int y = y0;

			for (int x = x0; x != x1 + incX; x += incX)
			{
				if (!funIsMoveable(x, y))
					return false;

				error += slope;

				if (error >= 0)
				{
					y += incY;
					error += errorInc;
				}
			}
		}
		else
		{
			// more vertical than horizontal
			int slope = 2 * dx;
			int error = -dy;
			int errorInc = -2 * dy;
			int x = x0;

			for (int y = y0; y != y1 + incY; y += incY)
			{
				if (!funIsMoveable(x, y))
					return false;

				error += slope;

				if (error >= 0)
				{
					x += incX;
					error += errorInc;
				}
			}
		}

		return true;
	}

	/*
	* Optimize three last step
	*/
	virtual void OptimizePriorityQuery(stCellIdxPF stIdxCur)
	{
		_stAStarGridCellPF* pCurCell, * pPrev1Cell, * pPrev2Cell;

		pCurCell = GetAStarGridCell(stIdxCur);

		pPrev1Cell = pCurCell ? pCurCell->pPrev : nullptr;

		pPrev2Cell = pPrev1Cell ? pPrev1Cell->pPrev : nullptr;

		if (!pCurCell || !pPrev1Cell || !pPrev2Cell)
			return;

		if (IsMoveCell2(pCurCell) &&
			IsMoveCell2(pPrev1Cell) &&
			IsMoveCell2(pPrev2Cell) &&
			IsDrawLine(pPrev2Cell, pCurCell))
		{
			pCurCell->pPrev = pPrev2Cell;
		}
	}

	virtual std::vector<stGridCellPF*> Execute(GridPF* pGridBoard, stCellIdxPF start, stCellIdxPF target)
	{
		_stAStarGridCellPF* pCellCur, * pNextCell, * pCellStart, * pCellTarget;
		float fDisNext2Dest, fDisTraveled = 0.f;
		std::vector<stGridCellPF*> path;

		stCellIdxPF stIdx;

		if (!Prepar(pGridBoard))
			return path;

		pCellCur = pCellStart = GetAStarGridCell(start);
		pCellTarget = GetAStarGridCell(target);

		UpdateWayPriority(start, target);

		PushToPriorityQuery(pCellStart, 0.f, 0.f, nullptr);

		size_t nLoop = 0, nMaxStep = pGridBoard->Length();

		while (pCellCur && nLoop++ <= nMaxStep)
		{
			if (pCellCur == pCellTarget)
				break;

			for (int i = 0; i < m_nWayDirection; i++)
			{
				if (m_arWayDirection[i].w > 0.0001)
				{
					stIdx.nX = pCellCur->pGrid->stIdx.nX + m_arWayDirection[i].x;
					stIdx.nY = pCellCur->pGrid->stIdx.nY + m_arWayDirection[i].y;

					pNextCell = GetAStarGridCell(stIdx);

					if (pNextCell == nullptr)
						continue;

					fDisTraveled = pCellCur->fDistanceSrc +
						(IsCrossCell(pCellCur->pGrid->stIdx, stIdx) ? 1.412f : 1.f);

					fDisNext2Dest = IsMoveable(pCellCur, pNextCell) && (pCellCur->pPrev != pNextCell) ?
						GetDistance(pNextCell, pCellTarget) : -1.f;

					if (fDisNext2Dest >= 0)
					{
						if (PushToPriorityQuery(pNextCell, fDisTraveled, fDisNext2Dest, pCellCur))
						{
							OptimizePriorityQuery(pNextCell->pGrid->stIdx);
						}
					}
				}
			}

			pCellCur = GetCellPriorityQuery();

			if (m_pFunPerform)
			{
				m_pFunPerform(m_GridCellUniqueManager, pCellCur->pGrid);
			}

			UpdateWayPriority(pCellCur->pGrid->stIdx, target);
		}

		// get path if exist
		if (pCellCur == pCellTarget)
		{
			path = GetPath(pCellTarget);
		}

		return path;
	}
};

#endif // XTHETASTAR_H
