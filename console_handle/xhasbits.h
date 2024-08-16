////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************
* Copyright (C) 2023-2024 thuong.nv <thuong.nv.mta@gmail.com>
* MIT software Licencs, see the accompanying
* http://www.opensource.org/licenses/mit-license.php
*
/***********************************************************************************
* @brief : Has bit data structer
* @file  : xhasbits.h
* @create: Aug 16, 2024
* @note  : For conditions of distribution and use, see copyright notice in readme.txt
***********************************************************************************/

#ifndef XHASBITS_H
#define XHASBITS_H

#include <type_traits>
#include <utility>

#ifndef MAX_HAS_BITS
#define MAX_HAS_BITS 5
#endif


/////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************/
// HasBits class

template<size_t doublewords, std::enable_if_t<(doublewords > 0 && doublewords <= MAX_HAS_BITS), bool> = true>
class HasBits {
public:
	HasBits() noexcept { Clear(); }

	void Clear() noexcept
	{
		memset(has_bits_, 0, sizeof(has_bits_));
	}

	__int32 operator[](int index)
	{
		return has_bits_[index];
	}

	const __int32& operator[](int index) const
	{
		return has_bits_[index];
	}

	bool operator==(const HasBits<doublewords>& rhs) const noexcept
	{
		return memcmp(has_bits_, rhs.has_bits_, sizeof(has_bits_)) == 0;
	}

	bool operator!=(const HasBits<doublewords>& rhs) const noexcept
	{
		return !(*this == rhs);
	}

	bool empty() const noexcept
	{
		for (size_t i = 0; i < doublewords; ++i)
			if (has_bits_[i]) return false;

		return true;
	}

private:
	__int32 has_bits_[doublewords];
};

template <>
inline bool HasBits<1>::empty() const noexcept
{
	return !has_bits_[0];
}

template <>
inline bool HasBits<2>::empty() const noexcept
{
	return !(has_bits_[0] | has_bits_[1]);
}

template <>
inline bool HasBits<3>::empty() const noexcept
{
	return !(has_bits_[0] | has_bits_[1] | has_bits_[2]);
}

template <>
inline bool HasBits<4>::empty() const noexcept
{
	return !(has_bits_[0] | has_bits_[1] | has_bits_[2] | has_bits_[3]);
}

template <>
inline bool HasBits<5>::empty() const noexcept
{
	return !(has_bits_[0] | has_bits_[1] | has_bits_[2] | has_bits_[3] | has_bits_[4]);
}

#endif // XHASBITS_H
