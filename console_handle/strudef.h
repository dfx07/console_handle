////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************
* Copyright (C) 2023-2024 thuong.nv <thuong.nv.mta@gmail.com>
* Distributed under the MIT software Licencs, see the accompanying
* File COPYING or http://www.opensource.org/licenses/mit-license.php
/**********************************************************************************/
/*
* @brief: struct define
* @file : studef.h
* @date : Aug 07, 2024
*/

#ifndef STRUDEF_H
#define STRUDEF_H

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

#endif // !STRUDEF_H

