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
#include <alg/xgridpf.h>


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
public:
	enum AStarWay {
		Four,
		Eight,
	};

	typedef std::set<stAStarGridCell*> AstarUniqueManager;
	typedef std::unordered_map<stGridCellPF*, stAStarGridCell> AstarMappingData;

public:
	virtual void SetWay(AStarWay eWay) noexcept
	{
		m_eWay = eWay;
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
		_stAStarGridCellPF* pCellUp, *pCellDown, *pCellLeft, *pCellRight;
		float fDisUp2Dest, fDisDown2Dest, fDisLeft2Dest, fDisRight2Dest;

		_stAStarGridCellPF* pCellCur;

		_stAStarGridCellPF* pCellStart = GetAStarGridCell(pGridBoard->Get(start));
		_stAStarGridCellPF* pCellTarget = GetAStarGridCell(pGridBoard->Get(target));

		pCellCur = pCellStart;

		PushToPriorityQuery(pCellStart, 0.f, 0.f, nullptr);

		auto funCalcDis = [](_stAStarGridCellPF* pC1, _stAStarGridCellPF* pC2) ->float
		{
			if (!pC1 || !pC2) return -1;

			float delY = float(pC2->pGrid->stIdx.nY - pC1->pGrid->stIdx.nY);
			float delX = float(pC2->pGrid->stIdx.nX - pC1->pGrid->stIdx.nX);
			return sqrtf(delX * delX + delY * delY);
		};

		auto funIsMoveable = [](_stAStarGridCellPF* pCell) ->float
		{
			if (!pCell) return false;

			bool bMove = false;

			if (pCell->pGrid->stCellData.fWeight <= 0)
			{
				bMove = true;
			}
			else
			{
				bMove = false;
			}

			return bMove;
		};

		float fDisTraveled = 0.f;
		size_t nLoop = 0, nMaxStep = pGridBoard->Length();

		while (pCellCur && nLoop++ <= nMaxStep)
		{
			if (pCellCur == pCellTarget)
				break;

			pCellUp    = GetAStarGridCell(pGridBoard->GetUp(pCellCur->pGrid->stIdx));
			pCellDown  = GetAStarGridCell(pGridBoard->GetDown(pCellCur->pGrid->stIdx));
			pCellLeft  = GetAStarGridCell(pGridBoard->GetLeft(pCellCur->pGrid->stIdx));
			pCellRight = GetAStarGridCell(pGridBoard->GetRight(pCellCur->pGrid->stIdx));

			fDisUp2Dest = funIsMoveable(pCellUp) && (pCellCur->pPrev != pCellUp) ?
				funCalcDis(pCellUp, pCellTarget) : -1.f;
			fDisDown2Dest = funIsMoveable(pCellDown) && (pCellCur->pPrev != pCellDown) ?
				funCalcDis(pCellDown, pCellTarget) : -1.f;
			fDisLeft2Dest = funIsMoveable(pCellLeft) && (pCellCur->pPrev != pCellLeft) ?
				funCalcDis(pCellLeft, pCellTarget) : -1.f;
			fDisRight2Dest = funIsMoveable(pCellRight) && (pCellCur->pPrev != pCellRight) ?
				funCalcDis(pCellRight, pCellTarget) : -1.f;

			fDisTraveled = pCellCur->fDistanceSrc + 1.f;

			if (fDisUp2Dest >= 0)
				PushToPriorityQuery(pCellUp, fDisTraveled, fDisUp2Dest, pCellCur);

			if (fDisDown2Dest >= 0)
				PushToPriorityQuery(pCellDown, fDisTraveled, fDisDown2Dest, pCellCur);

			if (fDisLeft2Dest >= 0)
				PushToPriorityQuery(pCellLeft, fDisTraveled, fDisLeft2Dest, pCellCur);

			if (fDisRight2Dest >= 0)
				PushToPriorityQuery(pCellRight, fDisTraveled, fDisRight2Dest, pCellCur);

			pCellCur = GetCellPriorityQuery();
		}

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
	AStarWay					m_eWay;
};


#endif // XASTAR_H
