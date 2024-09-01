////////////////////////////////////////////////////////////////////////////////////
/*!*********************************************************************************
*         Copyright (C) 2023-2024 thuong.nv <thuong.nv.mta@gmail.com>               
*                   MIT software Licencs, see the accompanying                      
************************************************************************************
* @brief : Grid board for pathfinding
* @file  : xgridpf.h
* @create: Aug 25, 2024
* @note  : For conditions of distribution and use, see copyright notice in readme.txt
***********************************************************************************/
#ifndef XGIRDPF_H
#define XGIRDPF_H

#include <vector>
#include <algorithm>

/////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************/
// Common struct

typedef struct _stIdx {
	int nX{ 0 };
	int nY{ 0 };
} stCellIdxPF;

typedef struct _stCellData
{
	float fWeight{ 0.f };
	void* pData{ nullptr };
} stCellDataPF;

typedef struct _stGridCell {
	stCellIdxPF	 stIdx;
	stCellDataPF stCellData;
} stGridCellPF;

/////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************/
// GridPF class

class GridPF
{
public:
	enum Step
	{
		Left,
		Right,
		Up,
		Down,
		LeftUp,
		L
	};

protected:
	typedef struct _stBoardInfo {
		unsigned int nRows{ 0 };
		unsigned int nCols{ 0 };
	} stBoardInfo;

protected:
	int GetIndex(const int x, const int y) const noexcept
	{
		if (x < 0 || y >= (int)m_BoardInfo.nRows ||
			y < 0 || x >= (int)m_BoardInfo.nCols)
			return -1;

		return x + m_BoardInfo.nCols * y;
	}

	void Rebuild() noexcept
	{
		int r = 0, c = 0;
		std::for_each(m_vecCells.begin(), m_vecCells.end(),
		[this, &r, &c](stGridCellPF& cell)
		{
			if (c >= m_BoardInfo.nCols) { r++; c = 0; }

			cell.stIdx = { c , r };
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

		for (auto i = 0; i < (int)m_BoardInfo.nRows; i++)
		{
			for (auto j = 0; j < (int)m_BoardInfo.nCols; j++)
			{
				nIdx = GetIndex(i, j);
				if (nIdx < 0 || nIdx >= szLength)
					continue;
				m_vecCells[nIdx].stIdx = { i , j };
				m_vecCells[nIdx].stCellData.pData = nullptr;
				m_vecCells[nIdx].stCellData.fWeight = vecWeights[nIdx];
			}
		}

		return true;
	}

	bool Make(std::vector<stGridCellPF>& vecCells, unsigned int rows, unsigned int cols)
	{
		if (vecCells.empty())
			return false;

		SetBoardSize(rows, cols);

		Rebuild();

		int nIdx = 0;
		size_t szLength = Length();

		for (int i = 0; i < vecCells.size(); i++)
		{
			nIdx = GetIndex(vecCells[i].stIdx.nX, vecCells[i].stIdx.nY);
			if (nIdx < 0 || nIdx >= szLength)
				continue;

			m_vecCells[nIdx].stCellData = vecCells[i].stCellData;
		}
	}

	bool MakeFrom(std::vector<stCellDataPF>& vecCells, unsigned int rows, unsigned int cols)
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
				m_vecCells[nIdx].stIdx = { i , j };
				m_vecCells[nIdx].stCellData = vecCells[nIdx];
			}
		}

		return true;
	}

public:
	stGridCellPF* Get(const int x, const int y) noexcept
	{
		int nIdx = GetIndex(x, y);
		if (nIdx < 0 || nIdx >= Length())
			return nullptr;

		return &m_vecCells[nIdx];
	}

	stGridCellPF* Get(stCellIdxPF& _idx) noexcept
	{
		int nIdx = GetIndex(_idx.nX, _idx.nY);
		if (nIdx < 0 || nIdx >= Length())
			return nullptr;

		return &m_vecCells[nIdx];
	}

	bool IsIdxErr(stCellIdxPF& idx) const noexcept
	{
		return (idx.nX == -1 && idx.nY == -1);
	}

	stGridCellPF* GetUp(stCellIdxPF& _idx) noexcept
	{
		stCellIdxPF idx;
		idx.nX = _idx.nX; idx.nY = _idx.nY - 1;
		return Get(idx);
	}

	stGridCellPF* GetRight(stCellIdxPF& _idx) noexcept
	{
		stCellIdxPF idx;
		idx.nX = _idx.nX + 1; idx.nY = _idx.nY;
		return Get(idx);
	}

	stGridCellPF* GetLeft(stCellIdxPF& _idx) noexcept
	{
		stCellIdxPF idx;
		idx.nX = _idx.nX - 1; idx.nY = _idx.nY;
		return Get(idx);
	}

	stGridCellPF* GetDown(stCellIdxPF& _idx) noexcept
	{
		stCellIdxPF idx;
		idx.nX = _idx.nX; idx.nY = _idx.nY + 1;
		return Get(idx);
	}

	stGridCellPF* GetStep()
	{

	}

	void SetData(const int x, const int y, stCellDataPF& cellData) noexcept
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
	stBoardInfo					m_BoardInfo;
	std::vector<stGridCellPF>	m_vecCells;
};



#endif //!XGIRDPF_H