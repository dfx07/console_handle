#pragma once

#include "console_type.h"


typedef struct ConsoleCellInfo
{
	void* pCustomerData;

} ConsoleCellInfo, *PConsoleCellInfo;


typedef struct ConsoleCellDraw
{
	ConsoleCellIndex m_idx;

	float m_fX;
	float m_fY;
	float m_fWidth;
	float m_fHeight;

	ConsoleColor m_Color;
	PConsoleCellInfo m_pInfo;

} ConsoleCellDraw, *PConsoleCellDraw;


class ConsoleBoardModelData
{
public:
	ConsoleBoardModelData() : m_nCols(0),
		m_nRows(0)
	{

	}

	~ConsoleBoardModelData()
	{
		Clear();
	}

protected:
	int GetIndex(const int nx, const int ny) const noexcept
	{
		return nx * m_nCols + ny;
	}

	bool IsValid(const int nIdx) const noexcept
	{
		return (nIdx >= 0 && nIdx < GetLength());
	}

public:
	void CreateBoardData()
	{
		for (int i = 0; i < m_nCols; i++)
		{
			for (int j = 0; j < m_nRows; j++)
			{
				CreateCellData(i, j);
			}
		}
	}

	PConsoleCellDraw CreateCellData(const int nx, const int ny)
	{
		int nIdx = GetIndex(nx, ny);

		if (!IsValid(nIdx))
			return nullptr;

		m_cells[nIdx] = new ConsoleCellDraw();

		if (!m_cells[nIdx])
		{
			_ASSERT(0);
			return nullptr;
		}

		m_cells[nIdx]->m_idx.m_iX = nx;
		m_cells[nIdx]->m_idx.m_iY = ny;

		return m_cells[nIdx];
	}

public:
	void RemoveData()
	{
		for (auto& pCell : m_cells)
		{
			delete pCell->m_pInfo;
			delete pCell;
			pCell = nullptr;
		}
	}

	void Clear()
	{
		m_nCols = 0;
		m_nRows = 0;

		RemoveData();
	}

	void SetSize(const int nRow, const int nCol)
	{
		Clear();

		m_nCols = nCol;
		m_nRows = nRow;

		int nLength = nRow * nCol;

		m_cells.resize(nLength);

		std::fill(m_cells.begin(), m_cells.end(), nullptr);
	}

	std::vector<PConsoleCellDraw>& Cells()
	{
		return m_cells;
	}

	PConsoleCellDraw GetCell(const int nrow, const int ncol)
	{
		int nIdx = GetIndex(nrow, ncol);

		if (!IsValid(nIdx))
			return nullptr;

		if (!m_cells[nIdx])
			return CreateCellData(nrow, ncol);

		return m_cells[nIdx];
	}

	bool SetCellInfo(const int nx, const int ny, PConsoleCellInfo pCellInfo)
	{
		int nIdx = nx * m_nCols + ny;
		if (nIdx < 0 || nIdx >= GetLength())
			return false;

		m_cells[nIdx]->m_pInfo = pCellInfo;

		return true;
	}

	PConsoleCellInfo GetCellInfo(const int nx, const int ny) const
	{
		int nIdx = GetIndex(nx, ny);

		if (!IsValid(nIdx))
			return nullptr;

		return m_cells[nIdx]->m_pInfo;
	}

	int GetLength() const noexcept
	{
		return static_cast<int>(m_cells.size());
	}

	int Columns() const noexcept
	{
		return m_nCols;
	}

	int Rows() const noexcept
	{
		return m_nRows;
	}

protected:
	int		m_nCols;
	int		m_nRows;
	std::vector<PConsoleCellDraw> m_cells;
};
