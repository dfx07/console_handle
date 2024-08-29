////////////////////////////////////////////////////////////////////////////////////
/*!*********************************************************************************
*         Copyright (C) 2023-2024 thuong.nv <thuong.nv.mta@gmail.com>               
*                   MIT software Licencs, see the accompanying                      
************************************************************************************
* @brief : Grid search data struct
* @file  : xgridsearch.h
* @create: Aug 18, 2024
* @note  : For conditions of distribution and use, see copyright notice in readme.txt
***********************************************************************************/
#ifndef XGRIDSEARCH_H
#define XGRIDSEARCH_H

#include <vector>

/////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************/
// GridSearch class

template<typename _Ty, typename _Tyd>
class GridSearch
{
	using object_type = _Ty;
	using data_type = _Tyd;

public:
	std::vector<object_type*> GetNearby(const data_type nX, const data_type nY) noexcept
	{
		std::vector<object_type*> vecNear;

		int nIdx = GetIndex(nX, nY);

		if (nIdx < m_nCol * m_nRow)
		{
			vecNear.push_back(m_vecGridData[nIdx]);
		}

		return vecNear;
	}

	void Update(object_type* pData, const data_type nX, const data_type nY) noexcept
	{
		int nIdx = GetIndex(nX, nY);

		if (nIdx < 0 || nIdx >= m_nCol * m_nRow)
		{
			assert(0);
			return;
		}

		m_vecGridData[nIdx] = pData;
	}

	void Create(const data_type nWidthView, const data_type nHeightView,
				const data_type nWidthCell, const data_type nHeightCell)
	{
		m_dtWidthView = nWidthView;
		m_dtHeightView = nHeightView;

		m_dtWidthCell = nWidthCell;
		m_dtHeighCell = nHeightCell;

		m_nCol = std::ceil(m_dtWidth / m_dtWidthCell);
		m_nRow = std::ceil(m_dtHeight / m_dtHeighCell);

		size_t szSize = m_nCol * m_nRow;
		m_vecGridData.resize(szSize);

		std::fill(m_vecGridData.begin(), m_vecGridData.end(), nullptr);
	}

protected:

	int GetIndex(const data_type nx, const data_type nY) const noexcept
	{
		int ic = std::floor(nx / m_dtWidthCell);
		int ir = std::floor(nY / m_dtHeighCell);

		return ir * m_nCol + ic;
	}

protected:
	data_type m_dtWidthCell, m_dtHeighCell;
	data_type m_dtWidthView, m_dtHeightView;

	int m_nCol;
	int m_nRow;

	std::vector<object_type*> m_vecGridData;
};

#endif // !XGRIDSEARCH_H


