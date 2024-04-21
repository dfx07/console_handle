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

public:
	void CreateBoardData()
	{
		int nIdx = 0;

		for (int i = 0; i < m_nCols; i++)
		{
			for (int j = 0; j < m_nRows; j++)
			{
				nIdx = i * m_nCols + j;

				m_cells[nIdx] = new ConsoleCellDraw();
				m_cells[nIdx]->m_idx.m_iX = i;
				m_cells[nIdx]->m_idx.m_iY = j;
			}
		}
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

	PConsoleCellDraw GetCell(const int nx, const int ny) const
	{
		int nIdx = nx * m_nCols + ny;
		if (nIdx < 0 || nIdx >= GetLength())
			return nullptr;

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
		int nIdx = nx * m_nCols + ny;
		if (nIdx < 0 || nIdx >= GetLength())
			return nullptr;

		return m_cells[nIdx]->m_pInfo;
	}

	int GetLength() const
	{
		return static_cast<int>(m_cells.size());
	}

	int Columns() const
	{
		return m_nCols;
	}

	int Rows() const
	{
		return m_nRows;
	}

protected:
	int		m_nCols;
	int		m_nRows;
	std::vector<PConsoleCellDraw> m_cells;
};
