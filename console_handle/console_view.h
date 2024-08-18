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


/////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************/
// View class
/* Coord
   View center view {0, 0};
*/


class View
{
public:
	View() : m_nWidthView(0), m_nHeightView(0), m_fZoomLevel(1.f)
	{

	}

	virtual ~View() { }

public:
	virtual void SetViewSize(const unsigned int nWidth, const unsigned int nHeight)
	{
		m_nWidthView = nWidth;
		m_nHeightView = nHeight;
	}

	virtual unsigned int GetWidth() const noexcept { return m_nWidthView; }
	virtual unsigned int GetHeight() const noexcept { return m_nHeightView; }
	virtual float GetZoomLevel() const noexcept { return m_fZoomLevel; }
	virtual ConsoleViewCoord GetCoordType() const noexcept { return m_eCoordType; }

public:
	virtual void* GetGraphics() const noexcept = 0;

protected:
	float					m_fZoomLevel{ 1.f };
	unsigned int			m_nWidthView{ 0 };
	unsigned int			m_nHeightView{ 0 };
	ConsoleGraphicsPtr		m_pGraphics{ nullptr };
	ConsoleViewCoord		m_eCoordType{ Center };
};

struct ConsoleViewProperty
{
	enum FLAG
	{
		SHOW_GRID  = 0x000001,
		GRID_COLOR = 0x000002,
	};

public:
	bool ValidFlag(int nFlag) const noexcept
	{
		return m_nFlagPro & nFlag;
	}

	void SetGridShow(bool bShow) noexcept
	{
		if (bShow)
		{
			m_nFlagPro |= FLAG::SHOW_GRID;
		}
		else
		{
			m_nFlagPro &= ~(FLAG::SHOW_GRID);
		}
	}

	bool IsGridShow() const noexcept { return ValidFlag(FLAG::SHOW_GRID); }

	void SetGridColor(ConsoleColor color) noexcept
	{
		m_nFlagPro |= FLAG::GRID_COLOR;
		m_clGridColor = color;
	}

	ConsoleColor GetGridColor() const noexcept
	{
		return m_clGridColor;
	}

public:
	ConsoleViewProperty()
	{
		m_nFlagPro |= FLAG::SHOW_GRID;
	}

protected:
	unsigned int m_nFlagPro{ 0 };
	ConsoleColor m_clGridColor{ 100, 100, 100 };
};

/////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************/
// ConsoleView class

class ConsoleView : public View
{
public:
	ConsoleView() : View()
	{
		m_fPadding = 0.f;
		m_pModelData = nullptr;
		InitData();
	}

	~ConsoleView() noexcept
	{

	}

public:
	virtual void* GetGraphics() const noexcept
	{
		if (m_pGraphics)
			return m_pGraphics.get();
		return nullptr;
	}


protected:
	void InitData()
	{
		m_pGraphics = std::make_shared<ConsoleGraphics>(this);
	}

public:
	void SetModelData(ConsoleBoardModelData* pBoard, bool bUpdateBoardData = true) noexcept
	{
		m_pModelData = pBoard;

		if (bUpdateBoardData)
		{
			UpdateBoardData();
		}
	}

	void SetPadding(const float fPadding) noexcept
	{
		m_fPadding = fPadding;
	}

	float GetWidthCell() const noexcept { return m_fWidthCell; }
	float GetHeightCell() const noexcept { return m_fHeightCell; }

	ConsoleCellIndex GetCell(const int xpos, const int ypos) const
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

	bool IsValidCellIndex(ConsoleCellIndex idx) const
	{
		if (idx.m_iX < 0 || idx.m_iY < 0)
			return false;

		if (idx.m_iX * m_pModelData->Columns() + idx.m_iY >= m_pModelData->GetLength())
			return false;

		return true;
	}

	ConsoleViewProperty* GetProperty() noexcept
	{
		return &m_Property;
	}

protected:

	void UpdateBoardData()
	{
		int i, j, nXS, nYS; float fx, fy;
		PConsoleCellDraw pCellDraw = nullptr;

		int nCols = m_pModelData->Columns();
		int nRows = m_pModelData->Rows();

		float fWidthCell = m_fWidthCell = float(m_nWidthView) / nCols;
		float fHeightCell = m_fHeightCell = float(m_nHeightView) / nRows;

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
	float						m_fWidthCell{ 0.f };
	float						m_fHeightCell{ 0.f };
	ConsoleBoardModelData*		m_pModelData{ nullptr };

	ConsoleViewProperty			m_Property;
};

/////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************/
// ConsoleGraphics class

interface ConsoleGraphicsIF
{
	virtual void SetActiveFont(ConsoleFontKey fontKey) = 0;
	virtual void SetTextCell(const int x, const int y, const ConsoleString& str, const ConsoleColor& col) = 0;
	virtual void SetColorCell(const int x, const int y, const ConsoleColor& col) = 0;
	virtual void SetBorderColor(const int x, const int y, const ConsoleColor& col) = 0;
};

class ConsoleGraphics : public ConsoleGraphicsIF
{
public:
	ConsoleGraphics(ConsoleView* pView)
		:m_pView(pView)
	{

	}

public:
	virtual void SetActiveFont(ConsoleFontKey fontKey)
	{
		m_ActiveFontKey = fontKey;
	}

	virtual void Clear()
	{
		m_DrawBuffer.ClearDrawBuffer();
	}

	virtual void SetTextCell(const int x, const int y, const ConsoleString& str, const ConsoleColor& col = { 255.f, 255.f, 255.f })
	{
		ConsoleCellDraw* pCellDraw = m_pModelData->GetCell(y, x);

		if (!pCellDraw)
			return;

		float r = pCellDraw->m_fX;
		float c = pCellDraw->m_fY;

		c += m_pView->GetHeightCell() / 3 - 1;

		m_DrawBuffer.OutText(ConsolePoint{ r, c }, str, col, m_ActiveFontKey);
	}

	virtual void SetColorCell(const int x, const int y, const ConsoleColor& col)
	{
		ConsoleCellDraw* pCellDraw = m_pModelData->GetCell(y, x);

		if (!pCellDraw)
			return;

		float r = pCellDraw->m_fX + 0.5f;
		float c = pCellDraw->m_fY + 0.5f;

		float width = pCellDraw->m_fWidth - 0.5f;
		float height = pCellDraw->m_fHeight - 0.5f;

		m_DrawBuffer.OutRectangle(ConsolePoint{ r, c }, width, height, col);
	}

	virtual void SetBorderColor(const int x, const int y, const ConsoleColor& col)
	{
		ConsoleCellDraw* pCellDraw = m_pModelData->GetCell(y, x);

		if (!pCellDraw)
			return;

		float r = pCellDraw->m_fX;
		float c = pCellDraw->m_fY;

		float width  = pCellDraw->m_fWidth;
		float height = pCellDraw->m_fHeight;

		m_DrawBuffer.IncreaseIndex(1);
		m_DrawBuffer.SkipIncreaseIndex(true);

		m_DrawBuffer.OutLine(ConsolePoint{ r, c }				  , ConsolePoint{ r + width, c }, col);
		m_DrawBuffer.OutLine(ConsolePoint{ r + width, c }		  , ConsolePoint{ r + width, c + height }, col);
		m_DrawBuffer.OutLine(ConsolePoint{ r + width, c + height }, ConsolePoint{ r, c + height }, col);
		m_DrawBuffer.OutLine(ConsolePoint{ r, c + height }		  , ConsolePoint{ r, c }, col);

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
		ConsoleViewProperty* pViewProperty = m_pView->GetProperty();

		if (!pViewProperty->IsGridShow())
			return true;

		if (!m_pModelData)
			return false;

		ConsoleCellDraw* pCell = nullptr;

		int nRows = m_pModelData->Rows();
		int nCols = m_pModelData->Columns();
		int r, c;

		m_BoardDrawBuffer.IncreaseIndex();
		m_BoardDrawBuffer.SkipIncreaseIndex(true);

		ConsoleColor clGridColor = pViewProperty->GetGridColor();

		for (r = 0; r < nRows; r++)
		{
			for (c = 0; c < nCols; c++)
			{
				pCell = m_pModelData->GetCell(r, c);

				if (!pCell)
					continue;

				m_BoardDrawBuffer.OutLine(ConsolePoint(pCell->m_fX, pCell->m_fY),
										  ConsolePoint{ pCell->m_fX + pCell->m_fWidth, pCell->m_fY },
										  clGridColor);
				m_BoardDrawBuffer.OutLine(ConsolePoint{ pCell->m_fX + pCell->m_fWidth, pCell->m_fY },
										  ConsolePoint{ pCell->m_fX + pCell->m_fWidth, pCell->m_fY + pCell->m_fHeight },
										  clGridColor);
				m_BoardDrawBuffer.OutLine(ConsolePoint{ pCell->m_fX + pCell->m_fWidth, pCell->m_fY + pCell->m_fHeight },
										  ConsolePoint{ pCell->m_fX, pCell->m_fY + pCell->m_fHeight },
										  clGridColor);
				m_BoardDrawBuffer.OutLine(ConsolePoint{ pCell->m_fX, pCell->m_fY + pCell->m_fHeight },
										  ConsolePoint{ pCell->m_fX, pCell->m_fY },
										  clGridColor);
			}
		}

		m_BoardDrawBuffer.SkipIncreaseIndex(false);

		return true;
	}

public:
	virtual void SetView(ConsoleView* pView) noexcept { m_pView = pView; }
	virtual ConsoleView* GetView() const noexcept { return m_pView; }
	virtual void SetModelData(ConsoleBoardModelData* pModelData) noexcept { m_pModelData = pModelData; }

protected:
	ConsoleBoardModelData*	m_pModelData{ nullptr };
	ConsoleView*			m_pView{ nullptr };

	ConsoleDrawBuffer		m_DrawBuffer;
	ConsoleDrawBuffer		m_BoardDrawBuffer;
	ConsoleFontKey			m_ActiveFontKey;

	friend class OpenGLConsoleDevice;
	friend class WinConsoleHandle;
};


#endif // CONSOLE_VIEW_H