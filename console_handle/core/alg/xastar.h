////////////////////////////////////////////////////////////////////////////////////
/*!*********************************************************************************
*         Copyright (C) 2023-2024 thuong.nv <thuong.nv.mta@gmail.com>               
*                   MIT software Licencs, see the accompanying                      
************************************************************************************
* @brief : a star pathfinding
* @file  : xastar.h
* @create: Aug 20, 2024
* @note  : For conditions of distribution and use, see copyright notice in readme.txt
***********************************************************************************/
#ifndef XASTAR_H
#define XASTAR_H

#include <set>
#include <unordered_map>
#include <queue>
#include <algorithm> 
#include <Windows.h>
#include "xpathfinder.h"


typedef struct _stAStarData
{
	float			fDistanceSrc{ 0 };
	float			fDistanceDst{ 0 };
	stGridCellPF* pPrev{ nullptr };
}stAStarData, * stAStarDataP;

typedef struct _stAStarGridCellPF {
	float		nIdx{ 0 };
	float		fDistanceSrc{ 0 };
	float		fDistanceDst{ 0 };
	_stAStarGridCellPF* pPrev{ nullptr };

	stGridCellPF* pGrid{ nullptr };
} stAStarGridCell;

typedef struct _ptrAStarGridCellCompare
{
	bool operator()(const stAStarGridCell* pC1, const stAStarGridCell* pC2) const
	{
		float fV1 = pC1->fDistanceSrc + pC1->fDistanceDst;
		float fV2 = pC2->fDistanceSrc + pC2->fDistanceDst;

		if (fabs(fV1 - fV2) < 0.001f)
			return (pC1->nIdx < pC2->nIdx);

		return fV1 > fV2;
	}
}ptrAStarDataCompare;


typedef std::priority_queue<stAStarGridCell*, std::vector<stAStarGridCell*>,
	_ptrAStarGridCellCompare> AstarCellPriorityQueue;

typedef void (*pFunAstarPerform)(AstarCellPriorityQueue&, _stAStarGridCellPF* );

class AStar : public PathFinding
{
	typedef struct tagWayDirectionalMove
	{
		int x{ 0 };
		int y{ 0 };

		float w{ 0.f };
	} WayDirectionalMove;

	static const int m_nWayDirection = 8;

public:
	enum WayDirectionMode {
		Four,
		Eight,
	};

	WayDirectionalMove m_arWayDirection[m_nWayDirection];
	const WayDirectionalMove m_arInitWayDirection[m_nWayDirection]
	{
		{-1, -1, 0.f}, // 0	: LeftUp
		{ 0, -1, 0.f}, // 1	: Up
		{ 1, -1, 0.f}, // 2	: RightUp
		{-1,  0, 0.f}, // 3	: Left
		{ 1,  0, 0.f}, // 4	: Right
		{-1,  1, 0.f}, // 5	: LeftDown
		{ 0,  1, 0.f}, // 6	: Down
		{ 1,  1, 0.f}, // 7	: RightDown
	};

	typedef std::set<stAStarGridCell*> AstarUniqueManager;
	typedef std::unordered_map<stGridCellPF*, stAStarGridCell> AstarMappingData;

public:
	void ResetWayDirection()
	{
		std::memcpy(m_arWayDirection, m_arInitWayDirection,
			m_nWayDirection * sizeof(WayDirectionalMove));
	}

	/*Normal vector {xDir, yDir}*/
	void UpdatePriority(stCellIdxPF& stStart, stCellIdxPF& stEnd)
	{
		float fUnix = float(stEnd.nX - stStart.nX);
		float fUniy = float(stEnd.nY - stStart.nY);

		float fLength = sqrtf(fUnix * fUnix + fUniy * fUniy);

		if (fLength >= 0.001f)
		{
			fUnix = fUnix / fLength;
			fUniy = fUniy / fLength;
		}
		else
		{
			fUnix = 0.f;
			fUniy = 0.f;
		}

		for (int i = 0; i < m_nWayDirection; i++)
		{
			if (std::fabs(m_arWayDirection[i].w) >= 0.001f)
			{
				m_arWayDirection[i].w =
					(m_arWayDirection[i].w + m_arWayDirection[i].x * fUnix +
					 m_arWayDirection[i].w + m_arWayDirection[i].y * fUniy);
			}
		}

		SortWayDirection();
	}


	void SortWayDirection()
	{
		std::sort(m_arWayDirection, m_arWayDirection + m_nWayDirection,
		[](WayDirectionalMove & a, WayDirectionalMove& b)
		{
			return a.w > b.w;
		});
	}

	void SetWayDirection(WayDirectionMode mode)
	{
		if (mode == WayDirectionMode::Four)
		{
			m_arWayDirection[1].w = 1.f;
			m_arWayDirection[3].w = 1.f;
			m_arWayDirection[4].w = 1.f;
			m_arWayDirection[6].w = 1.f;
		}
		else if (mode == WayDirectionMode::Eight)
		{
			for (int i = 0; i < m_nWayDirection; i++)
			{
				m_arWayDirection[i].w = 1.f;
			}
		}
	}

protected:

	virtual _stAStarGridCellPF* GetCellPriorityQuery()
	{
		if (m_CellPriorityQueue.empty())
			return nullptr;

		stAStarGridCell* pAstarCellCur = m_CellPriorityQueue.top();
		if (pAstarCellCur->pGrid == nullptr)
			return nullptr;

		m_CellPriorityQueue.pop();

		return pAstarCellCur;
	}

	virtual bool PushToPriorityQuery(_stAStarGridCellPF* pCell, float fDisSrcToCell, float fDisCell2Dest, _stAStarGridCellPF* pLinkPrev)
	{
		if (fDisSrcToCell < 0 || fDisCell2Dest < 0)
			return false;

		if (pCell == nullptr)
			return false;

		auto itFond = m_CellUniqueManager.find(pCell);

		if (itFond == m_CellUniqueManager.end())
		{
			pCell->fDistanceSrc = fDisSrcToCell;
			pCell->fDistanceDst = fDisCell2Dest;
			pCell->pPrev = pLinkPrev;
			m_CellPriorityQueue.push(pCell);
			m_CellUniqueManager.insert(pCell);

			return true;
		}
		else
		{
			if (pCell->fDistanceSrc + pCell->fDistanceDst > fDisSrcToCell + fDisCell2Dest)
			{
				pCell->fDistanceSrc = fDisSrcToCell;
				pCell->fDistanceDst = fDisCell2Dest;

				pCell->pPrev = pLinkPrev;
				return true;
			}
		}

		return false;
	};

	virtual std::vector<stGridCellPF*> GetPath(_stAStarGridCellPF* pCell)
	{
		std::vector<stGridCellPF*> path;
		path.reserve(100);

		if (pCell == nullptr)
			return path;

		stAStarGridCell* pAstarGridCellCur = pCell;

		do {
			path.push_back(pAstarGridCellCur->pGrid);
			pAstarGridCellCur = pAstarGridCellCur->pPrev;

		}while (pAstarGridCellCur != nullptr);

		std::reverse(path.begin(), path.end());

		return path;
	}

	virtual stAStarGridCell* GetAStarGridCell(stGridCellPF* pCell)
	{
		if (pCell == nullptr)
			return nullptr;

		stAStarGridCell* pAstarData = &m_GridDataMapping[pCell];
		if (!pAstarData->pGrid)
		{
			pAstarData->pGrid = pCell;
			pAstarData->nIdx = m_nIdxPriority++;
		}

		return pAstarData;
	}

private:

	virtual void Prepar(GridPF* pGridBoard)
	{
		Reset();
		m_GridDataMapping.reserve(pGridBoard->Size());
	}

	virtual void Reset()
	{
		m_CellPriorityQueue = AstarCellPriorityQueue();
		m_GridDataMapping.clear();
		m_nIdxPriority = 0;
	}

	virtual std::vector<stGridCellPF*> Execute(GridPF* pGridBoard, stCellIdxPF start, stCellIdxPF target)
	{
		_stAStarGridCellPF* pCellUp, *pCellDown, *pCellLeft, *pCellRight, *pNextCell;
		float fDisUp2Dest, fDisDown2Dest, fDisLeft2Dest, fDisRight2Dest, fDisNext2Dest;

		_stAStarGridCellPF* pCellCur;

		stCellIdxPF stIdx;

		_stAStarGridCellPF* pCellStart = GetAStarGridCell(pGridBoard->Get(start));
		_stAStarGridCellPF* pCellTarget = GetAStarGridCell(pGridBoard->Get(target));

		pCellCur = pCellStart;

		ResetWayDirection();

		SetWayDirection(pRefOption->m_bAllowCross ?
			WayDirectionMode::Eight : WayDirectionMode::Four);

		UpdatePriority(start, target);

		PushToPriorityQuery(pCellStart, 0.f, 0.f, nullptr);

		auto funCalcDis = [](_stAStarGridCellPF* pC1, _stAStarGridCellPF* pC2) ->float
		{
			if (!pC1 || !pC2) return -1;

			float delY = float(pC2->pGrid->stIdx.nY - pC1->pGrid->stIdx.nY);
			float delX = float(pC2->pGrid->stIdx.nX - pC1->pGrid->stIdx.nX);
			return sqrtf(delX * delX + delY * delY);
		};

		auto funIsCross = [](stCellIdxPF& stCur, stCellIdxPF& stNext) -> bool
		{
			return (stCur.nX != stNext.nX) && (stCur.nY != stNext.nY);
		};

		auto funIsMoveable = [&](_stAStarGridCellPF* _pCellCur, _stAStarGridCellPF* _pCellNext)
			->float
		{
			if (!_pCellNext) return false;
			if (_pCellNext->pGrid->stCellData.fWeight > 0)
				return false;

			if (funIsCross(_pCellCur->pGrid->stIdx, _pCellNext->pGrid->stIdx))
			{
				int delX = _pCellNext->pGrid->stIdx.nX - _pCellCur->pGrid->stIdx.nX;
				int delY = _pCellNext->pGrid->stIdx.nY - _pCellCur->pGrid->stIdx.nY;

				_stAStarGridCellPF* pCellCrs1 = GetAStarGridCell(pGridBoard->Get(_pCellCur->pGrid->stIdx.nX + delX, _pCellCur->pGrid->stIdx.nY));
				_stAStarGridCellPF* pCellCrs2 = GetAStarGridCell(pGridBoard->Get(_pCellCur->pGrid->stIdx.nX, _pCellCur->pGrid->stIdx.nY + delY));

				if (pCellCrs1 == nullptr || pCellCrs2 == nullptr)
					return (_pCellNext->pGrid->stCellData.fWeight > 0);

				return (pCellCrs1->pGrid->stCellData.fWeight <= 0 ||
						pCellCrs2->pGrid->stCellData.fWeight <= 0);
			}
			else
			{
				return !(_pCellNext->pGrid->stCellData.fWeight > 0);
			}
		};


		float fDisTraveled = 0.f;
		size_t nLoop = 0, nMaxStep = pGridBoard->Length();

		while (pCellCur && nLoop++ <= nMaxStep)
		{
			if (pCellCur == pCellTarget)
				break;

			for (int i = 0; i < m_nWayDirection; i++)
			{
				if (std::fabs(m_arWayDirection[i].w) >= 0.001f)
				{
					stIdx.nX = pCellCur->pGrid->stIdx.nX + m_arWayDirection[i].x;
					stIdx.nY = pCellCur->pGrid->stIdx.nY + m_arWayDirection[i].y;

					pNextCell = GetAStarGridCell(pGridBoard->Get(stIdx));

					if (pNextCell == nullptr)
						continue;

					fDisTraveled = pCellCur->fDistanceSrc +
						funIsCross(pCellCur->pGrid->stIdx, stIdx) ? 1.4142f : 1.f;

					fDisNext2Dest = funIsMoveable(pCellCur, pNextCell) && (pCellCur->pPrev != pNextCell) ?
						funCalcDis(pNextCell, pCellTarget) : -1.f;

					if (fDisNext2Dest >= 0)
						PushToPriorityQuery(pNextCell, fDisTraveled, fDisNext2Dest, pCellCur);
				}
			}

			pCellCur = GetCellPriorityQuery();

			if (m_pFunPerform)
			{
				m_pFunPerform(m_CellPriorityQueue, pCellCur);
			}

			Sleep(100);
		}

		//std::cout << "Traveled : " << m_CellUniqueManager.size() << std::endl;

		std::vector<stGridCellPF*> path;

		if (pCellCur == pCellTarget)
		{
			path = GetPath(pCellTarget);
			int d = 10;
		}

		return path;
	}

private:// internal

	AstarCellPriorityQueue		m_CellPriorityQueue;
	AstarUniqueManager			m_CellUniqueManager;
	AstarMappingData			m_GridDataMapping;
	int							m_nIdxPriority = 0;

public:// setup
	pFunAstarPerform			m_pFunPerform{nullptr};
	//AStarWay					m_eWay;
};


#endif // XASTAR_H
