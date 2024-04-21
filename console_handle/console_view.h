#pragma once

#include "console_model.h"

class ConsoleBoardView
{
public:
	ConsoleBoardView() : m_fPadding(0.f),
		m_nWidthView(0),
		m_nHeightView(0),
		m_pModelData(nullptr)
	{

	}

	ConsoleBoardView(int nCol, int nRow)
	{

	}

	~ConsoleBoardView()
	{

	}

public:
	void SetPadding(const float fPadding)
	{
		m_fPadding = fPadding;
	}

	void SetViewSize(const unsigned int nWidth, const unsigned int nHeight)
	{
		m_nWidthView = nWidth;
		m_nHeightView = nHeight;
	}

	ConsoleCellIndex GetCell(const int xpos, const int ypos)
	{
		if (!m_pModelData)
			return ConsoleCellIndex{ -1, -1 };

		auto vecCells = m_pModelData->Cells();

		for (auto& pCellDraw : vecCells)
		{
			if (xpos > pCellDraw->m_fX && xpos < pCellDraw->m_fX + pCellDraw->m_fWidth &&
				ypos > pCellDraw->m_fY && xpos < pCellDraw->m_fY + pCellDraw->m_fHeight)
				return pCellDraw->m_idx;
		}

		return ConsoleCellIndex{ -1, -1 };
	}

	bool IsValidCellIndex(ConsoleCellIndex& idx) const
	{
		if (idx.m_iX < 0 || idx.m_iY < 0)
			return false;

		if (idx.m_iX * m_pModelData->Columns() + idx.m_iY >= m_pModelData->GetLength())
			return false;

		return true;
	}

	void UpdateBoardData()
	{
		int nCols = m_pModelData->Columns();
		int nRows = m_pModelData->Rows();

		float fWidthCell = float(m_nWidthView) / nCols;
		float fHeightCell = float(m_nHeightView) / nRows;

		if (fWidthCell <= 0 || fHeightCell <= 0)
			return;

		float fx, fy;
		PConsoleCellDraw pCellDraw = nullptr;

		for (int i = 0; i < nCols; i++)
		{
			fx = i * fWidthCell + m_fPadding;

			for (int j = 0; j < nRows; j++)
			{
				fx = j * fHeightCell + m_fPadding;
				pCellDraw = m_pModelData->GetCell(i, j);

				pCellDraw->m_fX = fx;
				pCellDraw->m_fY = fy;
				pCellDraw->m_fWidth = fWidthCell;
				pCellDraw->m_fHeight = fHeightCell;
			}
		}
	}

	void SetModelData(ConsoleBoardModelData* pBoard)
	{
		m_pModelData = pBoard;
	}

	ConsoleGraphics* GetGraphics()
	{
		return m_pGraphics;
	}

protected:
	bool CreateBoardData()
	{
		m_pModelData->CreateBoardData();
	}

protected:
	float m_fPadding{ 0.f };
	unsigned int m_nWidthView{ 0 };
	unsigned int m_nHeightView{ 0 };

	ConsoleBoardModelData* m_pModelData{ nullptr };
	ConsoleGraphics* m_pGraphics{ nullptr };
};