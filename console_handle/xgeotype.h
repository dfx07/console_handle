////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************
* Copyright (C) 2023-2024 thuong.nv <thuong.nv.mta@gmail.com>
* MIT software Licencs, see the accompanying
* http://www.opensource.org/licenses/mit-license.php
*
/***********************************************************************************
* @brief : Geometry type define
* @file  : xgeotype.h
* @create: Aug 05, 2024
* @note  : For conditions of distribution and use, see copyright notice in readme.txt
***********************************************************************************/

#ifndef XGEOTYPE_H
#define XGEOTYPE_H

#include <type_traits>

#define _ZER0M(dst, _size) memset(dst, 0, _size)
#define _ZCOPM(dst, src, _size) memcpy(dst, src, _size)
#define _ZISER0(v, t) (std::abs(static_cast<t>(v)) <= static_cast <t>(0.0001))

template<class T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
struct _t2Tag {

	using type_value = _t2Tag<T>;

	_t2Tag() noexcept {
		_ZER0M(this, sizeof(type_value));
	}

	template<typename U, typename K>
	_t2Tag(const U& u, const K& k) noexcept {
		x = static_cast<T>(u);
		y = static_cast<T>(k);
	}

	template<typename U>
	_t2Tag(const _t2Tag<U>& sr) noexcept {
		x = static_cast<T>(sr.x);
		y = static_cast<T>(sr.y);
	}

	template<typename U>
	type_value& operator+(const _t2Tag<U>& sr) const noexcept {
		type_value ds;
		ds.x = static_cast<T>(x + sr.x);
		ds.y = static_cast<T>(y + sr.y);
		return ds;
	}

	template<typename U>
	const type_value& operator+=(const _t2Tag<U>& sr) noexcept {
		x += static_cast<T>(sr.x);
		y += static_cast<T>(sr.y);
		return *this;
	}

	template<typename U>
	const type_value& operator-(const _t2Tag<U>& sr) const noexcept {
		type_value ds;
		ds.x = static_cast<T>(x - sr.x);
		ds.y = static_cast<T>(y - sr.y);
		return ds;
	}

	template<typename U>
	const type_value& operator-=(const _t2Tag<U>& sr) noexcept {
		x -= static_cast<T>(sr.x);
		y -= static_cast<T>(sr.y);
		return *this;
	}

	template<typename U>
	const type_value& operator=(const _t2Tag<U>& sr) noexcept {

		x = static_cast<T>(sr.x);
		y = static_cast<T>(sr.y);
		return *this;
	}

	template<typename U>
	type_value operator/(const U& f) const noexcept {

		if (_ZISER0(f, T))
			return *this;

		type_value ds;
		ds.x = static_cast<T>(x / f);
		ds.y = static_cast<T>(y / f);
		return ds;
	}

	template<typename U>
	const type_value& operator/=(const U& f) noexcept {

		if (_ZISER0(f, T))
			return *this;

		x /= static_cast<T>(f);
		y /= static_cast<T>(f);
		return *this;
	}

	template<typename U>
	type_value& operator*(const U& f) const noexcept {
		type_value ds;
		ds.x = static_cast<T>(x * f);
		ds.y = static_cast<T>(y * f);
		return ds;
	}

	template<typename U>
	const type_value& operator*=(const U& f) noexcept {
		x *= static_cast<T>(f);
		y *= static_cast<T>(f);
		return *this;
	}

	template<typename U>
	const type_value& swap(_t2Tag<U>& sr) noexcept {
		auto _swap = [](T& a, U& b) {
			auto temp = a;
			a = static_cast<T>(b);
			b = static_cast<U>(temp);
		};

		_swap(x, sr.x);
		_swap(y, sr.y);

		return *this;
	}

public:
	union {
		struct {
			T x, y;
		};
		struct {
			T u, v;
		};
	};
};

template<class T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
struct _t3Tag {

	using type_value = _t3Tag<T>;

	_t3Tag() noexcept {
		_ZER0M(this, sizeof(type_value));
	}

	template<typename U, typename K = int, typename V = int>
	_t3Tag(const U& u, const K& k = 0, const V& v = 0) noexcept {
		x = static_cast<T>(u);
		y = static_cast<T>(k);
		z = static_cast<T>(v);
	}

	template<typename U>
	_t3Tag(const _t3Tag<U>& sr) {
		x = static_cast<T>(sr.x);
		y = static_cast<T>(sr.y);
		z = static_cast<T>(sr.z);
	}

	template<typename U>
	_t3Tag(const _t2Tag<U>& sr) {
		x = static_cast<T>(sr.x);
		y = static_cast<T>(sr.y);
		z = static_cast<T>(0);
	}

	template<typename U>
	type_value& operator+(const _t3Tag<U>& sr) const noexcept {
		type_value ds;
		ds.x = static_cast<T>(x + sr.x);
		ds.y = static_cast<T>(y + sr.y);
		ds.z = static_cast<T>(z + sr.z);

		return ds;
	}

	template<typename U>
	const type_value& operator+=(const _t3Tag<U>& sr) noexcept {
		x += static_cast<T>(sr.x);
		y += static_cast<T>(sr.y);
		z += static_cast<T>(sr.z);

		return *this;
	}

	template<typename U>
	const type_value& operator-(const _t3Tag<U>& sr) const noexcept {
		type_value ds;
		ds.x = static_cast<T>(x - sr.x);
		ds.y = static_cast<T>(y - sr.y);
		ds.z = static_cast<T>(z - sr.z);

		return ds;
	}

	template<typename U>
	const type_value& operator-=(const _t3Tag<U>& sr) noexcept {
		x -= static_cast<T>(sr.x);
		y -= static_cast<T>(sr.y);
		z -= static_cast<T>(sr.z);

		return *this;
	}

	template<typename U>
	const type_value& operator=(const _t3Tag<U>& sr) noexcept {

		x = static_cast<T>(sr.x);
		y = static_cast<T>(sr.y);
		z = static_cast<T>(sr.z);

		return *this;
	}

	template<typename U>
	type_value operator/(const U& f) const noexcept {

		if (_ZISER0(f, T))
			return *this;

		type_value ds;
		ds.x = static_cast<T>(x / f);
		ds.y = static_cast<T>(y / f);
		ds.z = static_cast<T>(z / f);

		return ds;
	}

	template<typename U>
	const type_value& operator/=(const U& f) noexcept {

		if (_ZISER0(f, T))
			return *this;

		x /= static_cast<T>(f);
		y /= static_cast<T>(f);
		z /= static_cast<T>(f);

		return *this;
	}

	template<typename U>
	type_value& operator*(const U& f) const noexcept {
		type_value ds;
		ds.x = static_cast<T>(x * f);
		ds.y = static_cast<T>(y * f);
		ds.z = static_cast<T>(z * f);

		return ds;
	}

	template<typename U>
	const type_value& operator*=(const U& f) noexcept {
		x *= static_cast<T>(f);
		y *= static_cast<T>(f);
		z *= static_cast<T>(f);

		return *this;
	}

	template<typename U>
	const type_value& swap(_t3Tag<U>& sr) noexcept {
		auto _swap = [](T& a, U& b) {
			auto temp = a;
			a = static_cast<T>(b);
			b = static_cast<U>(temp);
		};

		_swap(x, sr.x);
		_swap(y, sr.y);
		_swap(z, sr.z);

		return *this;
	}

public:
	union {
		struct {
			T x, y, z;
		};
		struct {
			T r, g, b;
		};
	};
};

#endif // !XGEOTYPE_H

