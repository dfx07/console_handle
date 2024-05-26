#pragma once

#include "console_model.h"
#include "console_device.h"

/*
View center view {0, 0};

*/

// Console graphic base
interface ConsoleGraphicsBase
{
public:
	virtual void Clear() = 0;
	virtual void DrawText(const int r, const int c, const TCHAR* str) = 0;
	virtual void DrawCellColor(const int r, const int c, float colr, float colg, float colb) = 0;

	virtual void SetDevice(ConsoleDevice* pDevice) { m_pDevice = pDevice; }
	virtual void SetModelData(ConsoleBoardModelData* pModelData) { m_pModelData = pModelData; }

protected:
	ConsoleBoardModelData* m_pModelData{ nullptr };
	ConsoleDevice* m_pDevice{ nullptr };
};

// Console graphic
class ConsoleGraphics : public ConsoleGraphicsBase
{
public:
	virtual void Clear()
	{
		if (!m_pDevice)
			return;

		m_pDevice->Clear();
		m_DrawBuffer.ClearDrawBuffer();
	}

	virtual void DrawText(const int r, const int c, const TCHAR* str)
	{
		ConsoleCellDraw* pCellDraw = m_pModelData->GetCell(r, c);

		if (!pCellDraw)
			return;

		float x = pCellDraw->m_fX;
		float y = pCellDraw->m_fY;

		m_DrawBuffer.OutText(ConsoleGpPoint{ x, y }, _T("1"), ConsoleGpColor{ 255.f, 255.f, 255.f });
	}

	virtual void DrawCellColor(const int r, const int c, float colr, float colg, float colb)
	{
		ConsoleCellDraw* pCellDraw = m_pModelData->GetCell(r, c);

		if (!pCellDraw)
			return;

		float x = pCellDraw->m_fX;
		float y = pCellDraw->m_fY;

		float width = pCellDraw->m_fWidth;
		float height = pCellDraw->m_fHeight;

		m_DrawBuffer.OutRectangle(ConsoleGpPoint{ x, y }, width, height, ConsoleGpColor{ colr, colg, colb });
	}

	ConsoleDrawBuffer* GetBufferData()
	{
		return &m_DrawBuffer;
	}

protected:

	virtual bool UpdateDrawBoardData()
	{
		m_BoardDrawBuffer.ClearDrawBuffer();

		if (!m_pModelData)
			return false;

		ConsoleCellDraw* pCellDraw = nullptr;

		int nRows = m_pModelData->Rows();
		int nCols = m_pModelData->Columns();

		for (int r = 0; r < nRows; r++)
		{
			for (int c = 0; c < nCols; c++)
			{
				pCellDraw = m_pModelData->GetCell(r, c);

				if (!pCellDraw)
					continue;

				m_BoardDrawBuffer.OutRectangle(ConsoleGpPoint{ pCellDraw->m_fX, pCellDraw->m_fY },
					pCellDraw->m_fWidth, pCellDraw->m_fHeight, ConsoleGpColor{ 255, 255, 255 });
			}
		}

		return true;
	}

	ConsoleDrawBuffer* GetBoardBufferData()
	{
		return &m_BoardDrawBuffer;
	}

protected:
	ConsoleDrawBuffer	m_DrawBuffer;
	ConsoleDrawBuffer	m_BoardDrawBuffer;

	friend class OpenGLConsoleDevice;
	friend class WinConsoleHandle;
};

typedef std::shared_ptr<ConsoleGraphics> ConsoleGraphicsPtr;

enum ConsoleBoardViewCoord
{
	TopLeft,
	Center,
};

// Console Board View
class ConsoleBoardView
{
public:
	ConsoleBoardView() : m_fPadding(0.f),
		m_nWidthView(0),
		m_nHeightView(0),
		m_pModelData(nullptr)
	{
		m_pGraphics = std::make_shared<ConsoleGraphics>();
	}

	~ConsoleBoardView() noexcept
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

	unsigned int GetWidth() noexcept
	{
		return m_nWidthView;
	}

	unsigned int GetHeight() noexcept
	{
		return m_nHeightView;
	}

	ConsoleCellIndex GetCell(const int xpos, const int ypos) const
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

	bool IsValidCellIndex(ConsoleCellIndex idx)
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
			nYS = -(int)m_nHeightView / 2;

			for (i = 0; i < nCols; i++)
			{
				fx = i * fWidthCell + m_fPadding + nXS;

				for (j = 0; j < nRows; j++)
				{
					fy = j * fHeightCell + m_fPadding + nYS;
					pCellDraw = m_pModelData->GetCell(i, j);

					pCellDraw->m_fX = fx;
					pCellDraw->m_fY = fy;
					pCellDraw->m_fWidth = fWidthCell;
					pCellDraw->m_fHeight = fHeightCell;
				}
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

	void SetModelData(ConsoleBoardModelData* pBoard) noexcept { m_pModelData = pBoard; }
	ConsoleGraphics* GetGraphics() const noexcept { return m_pGraphics.get(); }
	ConsoleBoardViewCoord GetCoordType() const noexcept { return m_eCoordType; }
	float GetZoomLevel() const noexcept { return m_fZoomLevel; }

protected:
	float						m_fPadding{ 0.f };
	unsigned int				m_nWidthView{ 0 };
	unsigned int				m_nHeightView{ 0 };
	ConsoleBoardViewCoord		m_eCoordType{ Center };
	float						m_fZoomLevel{ 1.f };

	ConsoleBoardModelData*		m_pModelData{ nullptr };
	ConsoleGraphicsPtr			m_pGraphics{ nullptr };
};