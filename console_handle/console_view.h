/*!*********************************************************************************
* Copyright (C) 2023-2024 thuong.nv <thuong.nv.mta@gmail.com>
* Distributed under the MIT software Licencs, see the accompanying
* File COPYING or http://www.opensource.org/licenses/mit-license.php
* 
* @file     console_view.h
* @create   june 23, 2024
* @brief    Console view
************************************************************************************/

#ifndef CONSOLE_VIEW_H
#define CONSOLE_VIEW_H

#include "console_def.h"
#include "console_model.h"
#include "console_device.h"
#include "console_font.h"

/*
View center view {0, 0};

*/

interface ConsoleGraphicsIF
{
	virtual void SetActiveFont(ConsoleFontKey fontKey) = 0;
	virtual void SetTextCell(const int r, const int c, const ConsoleString& str, const ConsoleColor& col) = 0;
	virtual void SetColorCell(const int r, const int c, const ConsoleColor& col) = 0;
	virtual void SetBorderColor(const int r, const int c, const ConsoleColor& col) = 0;
};

///////////////////////////////////////////////////////////////////////////////
/*ConsoleGraphic class*/

class ConsoleGraphics : public ConsoleGraphicsIF
{
public:
	virtual void SetActiveFont(ConsoleFontKey fontKey)
	{
		m_ActiveFontKey = fontKey;
	}

	virtual void Clear()
	{
		m_DrawBuffer.ClearDrawBuffer();
	}

	virtual void SetTextCell(const int r, const int c, const ConsoleString& str, const ConsoleColor& col = { 255.f, 255.f, 255.f })
	{
		ConsoleCellDraw* pCellDraw = m_pModelData->GetCell(r, c);

		if (!pCellDraw)
			return;

		float x = pCellDraw->m_fX;
		float y = pCellDraw->m_fY;

		m_DrawBuffer.OutText(ConsolePoint{ x, y }, str, col, m_ActiveFontKey);
	}

	virtual void SetColorCell(const int r, const int c, const ConsoleColor& col)
	{
		ConsoleCellDraw* pCellDraw = m_pModelData->GetCell(r, c);

		if (!pCellDraw)
			return;

		float x = pCellDraw->m_fX;
		float y = pCellDraw->m_fY;

		float width = pCellDraw->m_fWidth;
		float height = pCellDraw->m_fHeight;

		m_DrawBuffer.OutRectangle(ConsolePoint{ x, y }, width, height, col);
	}

	virtual void SetBorderColor(const int r, const int c, const ConsoleColor& col)
	{
		ConsoleCellDraw* pCellDraw = m_pModelData->GetCell(r, c);

		if (!pCellDraw)
			return;

		float x = pCellDraw->m_fX;
		float y = pCellDraw->m_fY;

		float width  = pCellDraw->m_fWidth;
		float height = pCellDraw->m_fHeight;

		m_DrawBuffer.IncreaseIndex(1);
		m_DrawBuffer.SkipIncreaseIndex(true);

		m_DrawBuffer.OutLine(ConsolePoint{ x, y }				  , ConsolePoint{ x + width, y }, col);
		m_DrawBuffer.OutLine(ConsolePoint{ x + width, y }		  , ConsolePoint{ x + width, y + height }, col);
		m_DrawBuffer.OutLine(ConsolePoint{ x + width, y + height }, ConsolePoint{ x, y + height }, col);
		m_DrawBuffer.OutLine(ConsolePoint{ x, y + height }		  , ConsolePoint{ x, y }, col);

		m_DrawBuffer.SkipIncreaseIndex(false);
	}

	ConsoleDrawBuffer* GetBufferData() noexcept
	{
		return &m_DrawBuffer;
	}

	ConsoleDrawBuffer* GetBoardBufferData() noexcept
	{
		return &m_BoardDrawBuffer;
	}

protected:

	virtual bool UpdateDrawBoardData()
	{
		m_BoardDrawBuffer.ClearDrawBuffer();

		if (!m_pModelData)
			return false;

		ConsoleCellDraw* pCell = nullptr;

		int nRows = m_pModelData->Rows();
		int nCols = m_pModelData->Columns();
		int r, c;

		m_BoardDrawBuffer.IncreaseIndex();
		m_BoardDrawBuffer.SkipIncreaseIndex(true);

		for (r = 0; r < nRows; r++)
		{
			for (c = 0; c < nCols; c++)
			{
				pCell = m_pModelData->GetCell(r, c);

				if (!pCell)
					continue;

				m_BoardDrawBuffer.OutLine(ConsolePoint(pCell->m_fX, pCell->m_fY),
										  ConsolePoint{ pCell->m_fX + pCell->m_fWidth, pCell->m_fY },
										  ConsoleColor{ 128, 128, 128 });
				m_BoardDrawBuffer.OutLine(ConsolePoint{ pCell->m_fX + pCell->m_fWidth, pCell->m_fY },
										  ConsolePoint{ pCell->m_fX + pCell->m_fWidth, pCell->m_fY + pCell->m_fHeight },
										  ConsoleColor{ 128, 128, 128 });
				m_BoardDrawBuffer.OutLine(ConsolePoint{ pCell->m_fX + pCell->m_fWidth, pCell->m_fY + pCell->m_fHeight },
										  ConsolePoint{ pCell->m_fX, pCell->m_fY + pCell->m_fHeight },
										  ConsoleColor{ 128, 128, 128 });
				m_BoardDrawBuffer.OutLine(ConsolePoint{ pCell->m_fX, pCell->m_fY + pCell->m_fHeight },
										  ConsolePoint{ pCell->m_fX, pCell->m_fY },
										  ConsoleColor{ 128, 128, 128 });
			}
		}

		m_BoardDrawBuffer.SkipIncreaseIndex(false);

		return true;
	}

public:
	virtual void SetDevice(ConsoleDevice* pDevice) { m_pDevice = pDevice; }
	virtual void SetModelData(ConsoleBoardModelData* pModelData) { m_pModelData = pModelData; }

protected:
	ConsoleBoardModelData*	m_pModelData{ nullptr };
	ConsoleDevice*			m_pDevice{ nullptr };

	ConsoleDrawBuffer		m_DrawBuffer;
	ConsoleDrawBuffer		m_BoardDrawBuffer;
	ConsoleFontKey			m_ActiveFontKey;

	friend class OpenGLConsoleDevice;
	friend class WinConsoleHandle;
};


template<typename T, typename DT>
class GridView
{
	typedef T object_type;
	typedef DT data_type;

public:
	std::vector<T*> GetNearby(const data_type nX, const data_type nY)
	{
		std::vector<T*> vecNear;

		int nIdx = GetIndex(nX, nY);

		if (nIdx < m_nCol * m_nRow)
		{
			vecNear.push_back(m_vecGridData[nIdx]);
		}

		return vecNear;
	}

	void Update(T* pObj, const data_type nX, const data_type nY)
	{
		int nIdx = GetIndex(nX, nY);

		if (nIdx < 0 || nIdx >= m_nCol * m_nRow)
		{
			assert(0);
			return;
		}

		m_vecGridData[nIdx] = pObj;
	}

protected:

	int GetIndex(const data_type nx, const data_type nY) const noexcept
	{
		int ic = std::floor(nx / m_dtWidthGrid);
		int ir = std::floor(nY / m_dtHeighGrid);

		return ir * m_nCol + ic;
	}

	void Create(const data_type nWidthView, const data_type nHeightView,
				const data_type nWidthCell, const data_type nHeightCell)
	{
		m_dtWidth = nWidthView;
		m_dtHeight = nHeightView;

		m_dtWidthGrid = nWidthCell;
		m_dtHeighGrid = nHeightCell;

		m_nCol = std::ceil(m_dtWidth / m_dtWidthGrid);
		m_nRow = std::ceil(m_dtHeight / m_dtHeighGrid);

		size_t szSize = m_nCol * m_nRow;
		m_vecGridData.resize(szSize);

		std::fill(m_vecGridData.begin(), m_vecGridData.end(), nullptr);
	}

protected:
	data_type m_dtWidthGrid;
	data_type m_dtHeighGrid;

	data_type m_dtWidth;
	data_type m_dtHeight;

	int m_nCol;
	int m_nRow;

	std::vector<object_type*> m_vecGridData;
};

class ConsoleView
{
public:
	ConsoleView() : m_nWidthView(0),
		m_nHeightView(0),
		m_fZoomLevel(1.f)
	{
		m_pGraphics = std::make_shared<ConsoleGraphics>();
	}

	virtual ~ConsoleView()
	{

	}

public:
	virtual void SetViewSize(const unsigned int nWidth, const unsigned int nHeight)
	{
		m_nWidthView = nWidth;
		m_nHeightView = nHeight;
	}

	unsigned int GetWidth() const noexcept
	{
		return m_nWidthView;
	}

	unsigned int GetHeight() const noexcept
	{
		return m_nHeightView;
	}

	virtual ConsoleCellIndex GetCell(const int xpos, const int ypos) const = 0;
	virtual bool IsValidCellIndex(ConsoleCellIndex idx) const = 0;

	float GetZoomLevel() const noexcept { return m_fZoomLevel; }
	ConsoleBoardViewCoord GetCoordType() const noexcept { return m_eCoordType; }

	ConsoleGraphics* GetGraphics() const noexcept 
	{
		if(m_pGraphics)
			return m_pGraphics.get();
		return nullptr;
	}

protected:

	float					m_fZoomLevel{ 1.f };
	unsigned int			m_nWidthView{ 0 };
	unsigned int			m_nHeightView{ 0 };
	ConsoleGraphicsPtr		m_pGraphics{ nullptr };
	ConsoleBoardViewCoord	m_eCoordType{ Center };
};

// Console Board View
class ConsoleBoardView : public ConsoleView
{
public:
	ConsoleBoardView() : ConsoleView(),
		m_fPadding(0.f),
		m_pModelData(nullptr)
	{

	}

	~ConsoleBoardView() noexcept
	{

	}

public:
	void SetModelData(ConsoleBoardModelData* pBoard) noexcept { m_pModelData = pBoard; }

	void SetPadding(const float fPadding) noexcept
	{
		m_fPadding = fPadding;
	}

	ConsoleCellIndex GetCell(const int xpos, const int ypos) const override
	{
		if (!m_pModelData)
			return ConsoleCellIndex{ -1, -1 };

		int xp = xpos, yp = ypos;

		if (m_eCoordType == Center)
		{
			xp = xpos - m_nWidthView / 2;
			yp = m_nHeightView / 2- ypos;
		}
		else if (m_eCoordType == TopLeft)
		{
			// The same
		}

		auto funcCheckInside = [&](PConsoleCellDraw _pCell) -> bool
		{
			if (m_eCoordType == Center)
			{
				if (xp >= _pCell->m_fX && xp <= _pCell->m_fX + _pCell->m_fWidth &&
					yp >= _pCell->m_fY && yp <= _pCell->m_fY + _pCell->m_fHeight)
					return true;
			}
			else
			{
				if (xp >= _pCell->m_fX && xp <= _pCell->m_fX + _pCell->m_fWidth &&
					yp >= _pCell->m_fY && yp <= _pCell->m_fY + _pCell->m_fHeight)
					return true;
			}
			return false;
		};

		auto vecCells = m_pModelData->Cells();

		for (auto& pCellDraw : vecCells)
		{
			if (funcCheckInside(pCellDraw))
			{
				return pCellDraw->m_idx;
			}
		}

		return ConsoleCellIndex{ -1, -1 };
	}

	bool IsValidCellIndex(ConsoleCellIndex idx) const override
	{
		if (idx.m_iX < 0 || idx.m_iY < 0)
			return false;

		if (idx.m_iX * m_pModelData->Columns() + idx.m_iY >= m_pModelData->GetLength())
			return false;

		return true;
	}

	void UpdateBoardData()
	{
		int i, j, nXS, nYS; float fx, fy;
		PConsoleCellDraw pCellDraw = nullptr;

		int nCols = m_pModelData->Columns();
		int nRows = m_pModelData->Rows();

		float fWidthCell = float(m_nWidthView) / nCols;
		float fHeightCell = float(m_nHeightView) / nRows;

		if (fWidthCell <= 0.f || fHeightCell <= 0.f)
			return;

		if (m_eCoordType == Center)
		{
			nXS = -(int)m_nWidthView / 2;
			nYS =  (int)m_nHeightView / 2;

			for (i = 0, fy = static_cast<float>(nYS); i < nRows; i++)
			{
				for (j = 0, fx = nXS - fWidthCell; j < nCols; j++)
				{
					fx = fx + fWidthCell + m_fPadding;
					pCellDraw = m_pModelData->GetCell(i, j);

					pCellDraw->m_fX = fx;
					pCellDraw->m_fY = fy - fHeightCell;
					pCellDraw->m_fWidth = fWidthCell;
					pCellDraw->m_fHeight = fHeightCell;
				}

				fy = fy - fHeightCell - m_fPadding;
			}
		}
		else if (m_eCoordType == TopLeft)
		{
			for (i = 0; i < nCols; i++)
			{
				fx = i * fWidthCell + m_fPadding;

				for (j = 0; j < nRows; j++)
				{
					fy = j * fHeightCell + m_fPadding;
					pCellDraw = m_pModelData->GetCell(i, j);

					pCellDraw->m_fX = fx;
					pCellDraw->m_fY = fy;
					pCellDraw->m_fWidth = fWidthCell;
					pCellDraw->m_fHeight = fHeightCell;
				}
			}
		}
	}



protected:
	float						m_fPadding{ 0.f };
	ConsoleBoardModelData*		m_pModelData{ nullptr };
};

#endif // CONSOLE_VIEW_H