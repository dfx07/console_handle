////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************
* Copyright (C) 2023-2024 thuong.nv <thuong.nv.mta@gmail.com>
* MIT software Licencs, see the accompanying
* http://www.opensource.org/licenses/mit-license.php
*
/***********************************************************************************
* @brief : a star pathfinding
* @file  : xastar.h
* @create: Aug 20, 2024
* @note  : For conditions of distribution and use, see copyright notice in readme.txt
***********************************************************************************/

#ifndef XASTAR_H
#define XASTAR_H

#include <vector>
#include <set>
#include <algorithm>
#include <unordered_map>


typedef struct _stIdx {
	int nX{ 0 };
	int nY{ 0 };
} stCellIdx;

typedef struct _stCellData
{
	float fWeight{ 0.f };
	void* pData{ nullptr };
} stCellData;

namespace pathfinding
{
	typedef struct _stBoardInfo {
		unsigned int nRows{ 0 };
		unsigned int nCols{ 0 };
	} stBoardInfo;

	typedef struct _stGridCell {
		stCellIdx	Idx;
		stCellData	stCellData;
	} stGridCell;

	typedef std::vector<stGridCell> stGridCellList;

	class GridBoard
	{
	protected:

		int GetIndex(const int x, const int y) const noexcept
		{
			if (x < 0 ||  y >= m_BoardInfo.nRows ||
				y < 0 ||  x >= m_BoardInfo.nCols)
				return -1;

			return x + m_BoardInfo.nCols * y;
		}

		void Rebuild() noexcept
		{
			int r = 0, c = 0;
			std::for_each(m_vecCells.begin(), m_vecCells.end(),
			[this, &r, &c](stGridCell& cell)
			{
				if (c >= m_BoardInfo.nCols) { r++; c = 0; }

				cell.Idx = { r , c };
				cell.stCellData.pData = nullptr;
				cell.stCellData.fWeight = 0.f;
				c++;
			});
		}

		void SetBoardSize(unsigned int rows, unsigned int cols, bool bRemake = false) noexcept
		{
			m_vecCells.clear();
			m_BoardInfo = { rows, cols };
			m_vecCells.resize(Size());

			if (bRemake)
				Rebuild();
		}

		void Clear()
		{
			m_vecCells.clear();
			m_BoardInfo = { 0, 0 };
		}

	public:
		bool MakeFrom(std::vector<float>& vecWeights, unsigned int rows, unsigned int cols)
		{
			if (vecWeights.size() < size_t(rows) * cols)
				return false;

			SetBoardSize(rows, cols);

			int nIdx = 0;
			size_t szLength = Length();

			for (int i = 0; i < m_BoardInfo.nRows; i++)
			{
				for (int j = 0; j < m_BoardInfo.nCols; j++)
				{
					nIdx = GetIndex(i, j);
					if (nIdx < 0 || nIdx >= szLength)
						continue;
					m_vecCells[nIdx].Idx = { i , j };
					m_vecCells[nIdx].stCellData.pData = nullptr;
					m_vecCells[nIdx].stCellData.fWeight = vecWeights[nIdx];
				}
			}

			return true;
		}

		bool MakeFrom(std::vector<stCellData>& vecCells, unsigned int rows, unsigned int cols)
		{
			if (vecCells.size() < size_t(rows) * cols)
				return false;

			SetBoardSize(rows, cols);

			int nIdx = 0;
			size_t szLength = Length();

			for (int i = 0; i < m_BoardInfo.nRows; i++)
			{
				for (int j = 0; j < m_BoardInfo.nCols; j++)
				{
					nIdx = GetIndex(i, j);
					if (nIdx < 0 || nIdx >= szLength)
						continue;
					m_vecCells[nIdx].Idx = { i , j };
					m_vecCells[nIdx].stCellData = vecCells[nIdx];
				}
			}

			return true;
		}

	public:

		stGridCell* Get(const int x, const int y) noexcept
		{
			int nIdx = GetIndex(x, y);
			if (nIdx < 0 || nIdx >= Length())
				return nullptr;

			return &m_vecCells[nIdx];
		}

		stGridCell* Get(stCellIdx& _idx) noexcept
		{
			int nIdx = GetIndex(_idx.nX, _idx.nY);
			if (nIdx < 0 || nIdx >= Length())
				return nullptr;

			return &m_vecCells[nIdx];
		}

		bool IsIdxErr(stCellIdx& idx) const noexcept
		{
			return (idx.nX == -1 && idx.nY == -1);
		}

		stGridCell* GetUp(stCellIdx& _idx) noexcept
		{
			stCellIdx idx;
			idx.nX = _idx.nX; idx.nY = _idx.nY - 1;
			return Get(idx);
		}

		stGridCell* GetRight(stCellIdx& _idx) noexcept
		{
			stCellIdx idx;
			idx.nX = _idx.nX + 1; idx.nY = _idx.nY;
			return Get(idx);
		}

		stGridCell* GetLeft(stCellIdx& _idx) noexcept
		{
			stCellIdx idx;
			idx.nX = _idx.nX - 1; idx.nY = _idx.nY;
			return Get(idx);
		}

		stGridCell* GetDown(stCellIdx& _idx) noexcept
		{
			stCellIdx idx;
			idx.nX = _idx.nX; idx.nY = _idx.nY + 1;
			return Get(idx);
		}

		void SetData(const int x, const int y, stCellData& cellData) noexcept
		{
			int nIdx = GetIndex(x, y);
			if (nIdx < 0 || nIdx >= Length())
				return;

			m_vecCells[nIdx].stCellData = cellData;
		}

		size_t Size() const noexcept { return (size_t)m_BoardInfo.nCols * m_BoardInfo.nRows; }
		size_t Length() const noexcept { return m_vecCells.size(); }
		int Rows() const noexcept { return m_BoardInfo.nRows; }
		int Cols() const noexcept { return m_BoardInfo.nCols; }

	protected:
		stBoardInfo		m_BoardInfo;
		stGridCellList	m_vecCells;
	};
}

using namespace pathfinding;

class PathFinding
{
public:
	virtual void Reset() = 0;
	virtual std::vector<stCellData*> Execute(stCellIdx start, stCellIdx target) = 0;
};

class AStar : public PathFinding
{
public:
	enum AStarWay {
		Four,
		Eight,
	};

	typedef struct _stAStarData
	{
		float		fDistanceSrc{ 0 };
		float		fDistanceDst{ 0 };
		stGridCell*	pPrev{ nullptr };
	}stAStarData, * stAStarDataP;

	typedef struct _ptrAStarDataCompare
	{
		bool operator()(_stGridCell const* pC1, _stGridCell const* pC2) const
		{
			stAStarDataP pD1 = reinterpret_cast<stAStarDataP>(pC1->pStrategyData);
			stAStarDataP pD2 = reinterpret_cast<stAStarDataP>(pC2->pStrategyData);

			return  (pD1->fDistanceSrc + pD1->fDistanceDst) <
					(pD2->fDistanceSrc + pD2->fDistanceDst);
		}
	}ptrAStarDataCompare;

public:
	virtual void SetWay(AStarWay eWay) noexcept
	{
		m_eWay = eWay;
	}

protected:
	virtual bool PushToPriorityQuery(stGridCell* pCell, float fSrcToDis, float fToTargetDis, stGridCell* pLinkPrev)
	{
		if (fToTargetDis < 0 || fSrcToDis < 0)
			return false;

		if (pCell == nullptr)
			return false;

		auto itFond = m_setPriority.insert(pCell);

		if (pCell->pStrategyData == nullptr)
		{
			pCell->pStrategyData = new stAStarData();
		}

		stAStarDataP pAstarData = reinterpret_cast<stAStarDataP>(pCell->pStrategyData);

		if (itFond.second == false)
		{
			if (pAstarData->fDistanceDst + pAstarData->fDistanceSrc > fSrcToDis + fToTargetDis)
			{
				pAstarData->fDistanceSrc = fSrcToDis;
				pAstarData->fDistanceDst = fToTargetDis;
				pAstarData->pPrev = pLinkPrev;
				return true;
			}
		}
		else
		{
			pAstarData->fDistanceSrc = fSrcToDis;
			pAstarData->fDistanceDst = fToTargetDis;
			pAstarData->pPrev = pLinkPrev;
			return true;
		}
		return false;
	};

	virtual std::vector<stGridCell*> GetPath(stGridCell* pCell) const
	{
		std::vector<stGridCell*> path;
		path.reserve(100);

		stGridCell* pCellCur = pCell;
		path.push_back(pCellCur);

		while (pCellCur->pStrategyData != nullptr)
		{
			pCellCur = reinterpret_cast<stAStarDataP>(pCellCur->pStrategyData)->pPrev;
			path.push_back(pCellCur);
		}

		std::reverse(path.begin(), path.end());

		return path;
	}

public:

	virtual void Prepar(GridBoard* pGridBoard)
	{
		m_mapIndexData.clear();
		m_mapIndexData.reserve(pGridBoard->Size());
	}

	virtual void Reset()
	{
		m_setPriority.clear();
	}

	virtual std::vector<stCellData*> Execute(GridBoard* pGridBoard, stCellIdx start, stCellIdx target)
	{
		stGridCell* pCellUp, *pCellDown, *pCellLeft, *pCellRight, *pCellCur, *pCellNext;
		float fDisUp, fDisDown, fDisLeft, fDisRight;
		float fTotalDisUp, fTotalDisDown, fTotalDisLeft, fTotalDisRight;

		stGridCell* pCellStart = pGridBoard->Get(start);
		stGridCell* pCellTarget = pGridBoard->Get(target);

		pCellCur = pCellStart;

		int nMaxStep = pGridBoard->Length();

		m_setPriority.insert(pCellStart);

		auto funCalcDis = [](stGridCell* pC1, stGridCell* pC2) ->float
		{
			if (!pC1 || !pC2) return -1;

			float delY = (pC2->Idx.nY - pC1->Idx.nY);
			float delX = (pC2->Idx.nX - pC1->Idx.nX);
			return sqrtf(delX * delX + delY * delY);
		};

		auto funIsMoveable = [](stGridCell* pCell) ->float
		{
			if (!pCell) return false;

			return (pCell->stCellData.fWeight <= 0);
		};

		stAStarDataP pAtarDataCur, pAtarDataDown, pAtarDataLeft, pAtarDataRight;
		float fDistanceTraveled = 0.f;

		bool bFound = false;
		int nLoop = 0;

		while (nLoop++ <= nMaxStep)
		{
			if (pCellCur == pCellTarget)
			{
				bFound = true;
				break;
			}

			pCellUp    = pGridBoard->GetUp(pCellCur->Idx);
			pCellDown  = pGridBoard->GetDown(pCellCur->Idx);
			pCellLeft  = pGridBoard->GetLeft(pCellCur->Idx);
			pCellRight = pGridBoard->GetRight(pCellCur->Idx);

			fDisUp     = funIsMoveable(pCellUp)   ? funCalcDis(pCellUp, pCellTarget) : -1.f;
			fDisDown   = funIsMoveable(pCellDown) ? funCalcDis(pCellDown, pCellTarget) : -1.f;
			fDisLeft   = funIsMoveable(pCellLeft) ? funCalcDis(pCellLeft, pCellTarget) : -1.f;
			fDisRight  = funIsMoveable(pCellRight)? funCalcDis(pCellRight, pCellTarget) : -1.f;

			pAtarDataCur = m_mapIndexData[pCellCur->Idx];
			fDistanceTraveled = pAtarDataCur->fDistanceSrc;

			PushToPriorityQuery(pCellUp, fDistanceTraveled + 1.f, fDisUp, pCellCur);

			PushToPriorityQuery(pCellDown, fDistanceTraveled + 1.f, fDisDown, pCellCur);

			PushToPriorityQuery(pCellLeft, fDistanceTraveled + 1.f, fDisLeft, pCellCur);

			PushToPriorityQuery(pCellRight, fDistanceTraveled + 1.f, fDisRight, pCellCur);

			if (m_setPriority.empty())
				break;

			pCellCur = *m_setPriority.begin();
			m_setPriority.erase(m_setPriority.begin());
		}

		std::vector<stGridCell*> path;

		if (bFound)
		{
			path = GetPath(pCellTarget);
		}

		return std::vector<stCellData*>();

		
	}

private:

	ptrAStarDataCompare m_cmp;

	std::set<stGridCell*, ptrAStarDataCompare>  m_setPriority;
	std::unordered_map<stCellIdx, stAStarDataP> m_mapIndexData;
	AStarWay m_eWay;
};


class PathFinder
{
public:
	void SetGridBoard(GridBoard* pGridBoard)
	{
		m_pGridBoard = pGridBoard;
	}

	void SetPathFinding(PathFinding* pPathFinding)
	{
		m_pStrategy = pPathFinding;
	}

	virtual std::vector<stCellData*> Search(stCellIdx start, stCellIdx target)
	{
		std::vector<stCellData*> vePath;
		if (m_pStrategy)
			return vePath;

		return m_pStrategy->Execute(start, target);
	}

private:
	GridBoard*	 m_pGridBoard;
	PathFinding* m_pStrategy;
};


#endif // XASTAR_H
