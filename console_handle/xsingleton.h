////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************
* Copyright (C) 2023-2024 thuong.nv <thuong.nv.mta@gmail.com>
* MIT software Licencs, see the accompanying
* http://www.opensource.org/licenses/mit-license.php
*
/***********************************************************************************
* @brief : Design pattern singleton
* @file  : xsingleton.h
* @create: Aug 07, 2024
* @note  : For conditions of distribution and use, see copyright notice in readme.txt
***********************************************************************************/

#ifndef XSINGLETON_H
#define XSINGLETON_H

/* Usage : inheritance
*  Ex : class ClassABC : public singleton<ClassABC> {
*          friend class singleton<ClassABC>;
* private:
*          ClassABC(){};
* };
*/
template<typename _Ty>
class singleton {

protected:
	singleton() = default;
	~singleton() = default;
	singleton(const singleton&) = delete;
	singleton(singleton&&) = delete;
	singleton& operator=(singleton const&) = delete;

public:
	static _Ty* instance() {
		static _Ty instance;
		return &instance;
	}
};

#endif // !XSINGLETON_H

