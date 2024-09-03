////////////////////////////////////////////////////////////////////////////////////
/*!*********************************************************************************
*         Copyright (C) 2023-2024 thuong.nv <thuong.nv.mta@gmail.com>
*                   MIT software Licencs, see the accompanying
************************************************************************************
* @brief : Path finder (a-star)
* @file  : xpathfinder.h
* @create: Aug 25, 2024
* @note  : For conditions of distribution and use, see copyright notice in readme.txt
***********************************************************************************/
#ifndef XPATH_FINDER
#define XPATH_FINDER

#include "xgridpf.h"
#include "alg/xastar.h"

struct PathFinderOption
{
	bool m_bDontCrossCorners{ false };
	bool m_bAllowCross{ true };
};

/////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************/
// PathFinding class

class PathFinding
{
public:
	virtual void SetOption(PathFinderOption* pOption) noexcept
	{
		pRefOption = pOption;
	}

protected:
	virtual void Reset() = 0;

	virtual std::vector<stGridCellPF*> Execute(GridPF* pGridBoard, stCellIdxPF start, stCellIdxPF target) = 0;

protected:
	PathFinderOption* pRefOption{ nullptr };

	friend class PathFinder;
};

/////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************/
// PathFinder class

class PathFinder
{
public:	// Set option

	void SetOptionAllowCross(bool bAllow) noexcept
	{
		m_Option.m_bAllowCross = bAllow;
	}

	void SetOptionDontCrossCorners(bool bAllow) noexcept
	{
		m_Option.m_bDontCrossCorners = bAllow;
	}

public:
	void Prepar(GridPF* pGridBoard, PathFinding* pPathFinding) noexcept
	{
		if (!pGridBoard || !pPathFinding)
		{
			assert(0);
			return;
		}

		m_pGridBoard = pGridBoard;
		m_pStrategy = pPathFinding;
	}

	virtual std::vector<stGridCellPF*> Search(stCellIdxPF start, stCellIdxPF target)
	{
		std::vector<stGridCellPF*> vePath;
		if (!m_pStrategy)
			return vePath;

		m_pStrategy->SetOption(&m_Option);

		return m_pStrategy->Execute(m_pGridBoard, start, target);
	}

private:
	GridPF*				m_pGridBoard;
	PathFinding*		m_pStrategy;

	PathFinderOption	m_Option;
};


#endif // XPATH_FINDER