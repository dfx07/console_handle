////////////////////////////////////////////////////////////////////////////////////
/*!*********************************************************************************
*         Copyright (C) 2023-2024 thuong.nv <thuong.nv.mta@gmail.com>               
*                   MIT software Licencs, see the accompanying                      
************************************************************************************
* @brief : algorithm utility
* @file  : xalgutils.h
* @create: Aug 06, 2024
* @note  : For conditions of distribution and use, see copyright notice in readme.txt
***********************************************************************************/
#ifndef XALGUTIL_H
#define XALGUTIL_H

#include <algorithm>

namespace util
{
	template<typename _Ty>
	using __IsNumber = std::enable_if_t<std::is_arithmetic_v<_Ty>, bool>;

	template<typename _Ty>
	_Ty clamp(const _Ty& v, const _Ty& min, const _Ty& max) noexcept
	{
		if (v < min) return min;
		if (v > max) return max;
		return v;
	}

	template<typename _Ty>
	_Ty map(const _Ty& v, const _Ty& rmin1, const _Ty& rmax1, const _Ty& rmin2, const _Ty& rmax2) noexcept
	{
		auto _v = clamp<_Ty>(v, rmin1, rmax1);

		if (_v <= rmin1) return rmin2;
		if (_v >= rmax1) return rmax2;

		if (std::abs(rmax1 - rmin1) <= 0.0001)
			return rmin2;
		return rmin2 + (v - rmin1) * (rmax2 - rmin2) / (rmax1 - rmin1);
	}

	template<typename _Ty, __IsNumber<_Ty> = true>
	bool check_bit(_Ty& value, const int idx) noexcept
	{
		return value & (1U << (idx));
	}

	template<typename _Ty, __IsNumber<_Ty> = true>
	void set_bit(_Ty& value, const int idx) noexcept
	{
		value |= (1U << (idx));
	}

	template<typename _Ty, __IsNumber<_Ty> = true>
	void clear_bit(_Ty& value, const int idx) noexcept
	{
		value &= ~(1U << (idx));
	}

	template<typename _Ty, __IsNumber<_Ty> = true>
	void flip_bit(_Ty& value, const int idx) noexcept
	{
		value ^= (1U << (idx));
	}

	template<typename _Ty, __IsNumber<_Ty> = true>
	void set_mask_bit(_Ty& value, const _Ty& bitset)
	{
		value |= bitset;
	}

	template<typename _Ty, __IsNumber<_Ty> = true>
	void clear_mask_bit(_Ty& value, const _Ty& bitset)
	{
		value &= ~bitset;
	}

	template<typename _Ty, __IsNumber<_Ty> = true>
	void flip_mask_bit(_Ty& value, const _Ty& bitset) noexcept
	{
		value ^= bitset;
	}
};

#endif // !XALGUTIL_H

