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

typedef struct _stCell {
	stCellIdxPF	 stIdx;
	stCellDataPF stData;
} stCellPF;

typedef struct _stGridInfo {
	unsigned int nRows{ 0 };
	unsigned int nCols{ 0 };
} stGridPFInfo;

/////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************/
// GridPF class

class GridPF
{
protected:
	int GetIndex(const int x, const int y) const noexcept
	{
		if (x < 0 || y >= (int)m_GridInfo.nRows ||
			y < 0 || x >= (int)m_GridInfo.nCols)
			return -1;

		return x + m_GridInfo.nCols * y;
	}

	void Rebuild() noexcept
	{
		int r = 0, c = 0;
		std::for_each(m_vecCells.begin(), m_vecCells.end(),
		[this, &r, &c](stCellPF& cell)
		{
			if (c >= (int)m_GridInfo.nCols) { r++; c = 0; }

			cell.stIdx = { c , r };
			cell.stData.pData = nullptr;
			cell.stData.fWeight = 0.f;
			c++;
		});
	}

	void SetBoardSize(unsigned int rows, unsigned int cols, bool bRemake = false) noexcept
	{
		m_vecCells.clear();
		m_GridInfo = { rows, cols };
		m_vecCells.resize(Size());

		if (bRemake)
			Rebuild();
	}

	void Clear()
	{
		m_vecCells.clear();
		m_GridInfo = { 0, 0 };
	}

public:
	bool BuildFrom(std::vector<float>& vecWeights, unsigned int rows, unsigned int cols)
	{
		if (vecWeights.size() < size_t(rows) * cols)
			return false;

		SetBoardSize(rows, cols);

		int nIdx = 0;
		size_t szLength = Length();

		for (auto i = 0; i < (int)m_GridInfo.nRows; i++)
		{
			for (auto j = 0; j < (int)m_GridInfo.nCols; j++)
			{
				nIdx = GetIndex(i, j);
				if (nIdx < 0 || nIdx >= szLength)
					continue;
				m_vecCells[nIdx].stIdx = { i , j };
				m_vecCells[nIdx].stData.pData = nullptr;
				m_vecCells[nIdx].stData.fWeight = vecWeights[nIdx];
			}
		}

		return true;
	}

	bool BuildFrom(std::vector<stCellPF>& vecCells, unsigned int rows, unsigned int cols)
	{
		SetBoardSize(rows, cols);

		Rebuild();

		int nIdx = 0;
		size_t szLength = Length();

		for (int i = 0; i < vecCells.size(); i++)
		{
			nIdx = GetIndex(vecCells[i].stIdx.nX, vecCells[i].stIdx.nY);
			if (nIdx < 0 || nIdx >= szLength)
				continue;

			m_vecCells[nIdx].stData = vecCells[i].stData;
		}

		return true;
	}

	bool BuildFrom(std::vector<stCellDataPF>& vecCells, unsigned int rows, unsigned int cols)
	{
		if (vecCells.size() < size_t(rows) * cols)
			return false;

		SetBoardSize(rows, cols);

		int nIdx = 0;
		size_t szLength = Length();

		for (int i = 0; i < (int)m_GridInfo.nRows; i++)
		{
			for (int j = 0; j < (int)m_GridInfo.nCols; j++)
			{
				nIdx = GetIndex(i, j);
				if (nIdx < 0 || nIdx >= szLength)
					continue;
				m_vecCells[nIdx].stIdx = { i , j };
				m_vecCells[nIdx].stData = vecCells[nIdx];
			}
		}

		return true;
	}

public:
	stCellPF* Get(const int x, const int y) noexcept
	{
		int nIdx = GetIndex(x, y);
		if (nIdx < 0 || nIdx >= Length())
			return nullptr;

		return &m_vecCells[nIdx];
	}

	stCellPF* Get(const stCellIdxPF& _idx) noexcept
	{
		int nIdx = GetIndex(_idx.nX, _idx.nY);
		if (nIdx < 0 || nIdx >= Length())
			return nullptr;

		return &m_vecCells[nIdx];
	}

	void SetData(const int x, const int y, stCellDataPF& cellData) noexcept
	{
		int nIdx = GetIndex(x, y);
		if (nIdx < 0 || nIdx >= Length())
			return;

		m_vecCells[nIdx].stData = cellData;
	}

	size_t Size() const noexcept { return (size_t)m_GridInfo.nCols * m_GridInfo.nRows; }
	size_t Length() const noexcept { return m_vecCells.size(); }
	int Rows() const noexcept { return m_GridInfo.nRows; }
	int Cols() const noexcept { return m_GridInfo.nCols; }

protected:
	stGridPFInfo			m_GridInfo;
	std::vector<stCellPF>	m_vecCells;
};



#endif //!XGIRDPF_H