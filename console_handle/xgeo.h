////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************
* Copyright (C) 2023-2024 thuong.nv <thuong.nv.mta@gmail.com>
* Distributed under the MIT software Licencs, see the accompanying
* File COPYING or http://www.opensource.org/licenses/mit-license.php
/**********************************************************************************/
/*
* @brief: geometry
* @file : xgeo.h
* @date : Aug 06, 2024
*/

#ifndef XGEO_H
#define XGEO_H

#include "typedef.h"

#define _USE_FLOAT

#define _MIN_EPSILON 0.01
#define _MID_EPSILON 0.001
#define _MAX_EPSILON 0.0001


#define EPSILON _MID_EPSILON

#ifdef _USE_FLOAT
typedef float _float;
#define _sqrt(p) std::sqrtf(p)
#define _cos(p) std::cosf(p)
#define _sin(p) std::sinf(p)
#else
typedef double _float;
#define _sqrt(p) std::sqrt(p)
#define _cos(p) std::cos(p)
#define _sin(p) std::sin(p)
#endif


namespace geo {

	template <typename _Ty>
	using Vec2D = _t2Tag<_Ty>;
	using Vec2D_F = _t2Tag<_float>;

	constexpr _float PI = static_cast<_float>(3.14159265359);

	/* Convert degree to radian*/
	template<typename _Ty>
	_float deg2rad(const _Ty& deg) {
		return deg * (PI / 180);
	}

	/* Convert radian to degree*/
	template<typename _Ty>
	_float rad2deg(const _Ty& rad) {
		return rad * (180/ PI);
	}

	/* calculate magnitude*/
	template<typename _Ty>
	_float mag(const Vec2D<_Ty>& v) {
		return _sqrt(static_cast<_float>(v.x * v.x + v.y * v.y));
	}

	/* calculate unit vector*/
	template<typename _Ty>
	Vec2D_F normal(const Vec2D<_Ty>& v) {

		_float fMagnitude = geo::mag(v);
		if (fMagnitude <= EPSILON)
			return Vec2D_F(0, 0);

		return Vec2D_F(v.x / fMagnitude, v.y / fMagnitude);
	}

	/* move point use unit vector and distance*/
	template<typename _Ty, typename _Ty2>
	Vec2D_F move(const Vec2D<_Ty>& pt, const Vec2D<_Ty2>& vn, const _float fDistance) {

		Vec2D_F ptMove;

		ptMove.x = pt.x + vn.x * fDistance;
		ptMove.y = pt.y + vn.y * fDistance;

		return ptMove;
	}

	/* rotate vector with degree*/
	template<typename _Ty>
	Vec2D_F rotate(const Vec2D<_Ty>& v, const float fDegree){
		_float fRadAngle = deg2rad(fDegree);

		Vec2D_F vRoate;
		vRoate.x = _cos(fRadAngle) * v.x - _sin(fRadAngle) * v.y;
		vRoate.y = _sin(fRadAngle) * v.x + _cos(fRadAngle) * v.y;
		return vRoate;
	}

	/* calculate dot product*/
	template<typename _Ty>
	_float dot(const Vec2D<_Ty>& v1, const Vec2D<_Ty>& v2)
	{
		return v1.x * v2.x + v1.y + v2.y;
	}

	/* calculate cross product*/
	template<typename _Ty>
	_float cross(const Vec2D<_Ty>& v1, const Vec2D<_Ty>& v2)
	{
		return (v1.x * v2.y) - (v1.y * v2.x);
	}
}

#endif // !XGEO_H
