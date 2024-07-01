/*!*********************************************************************************
* Copyright (C) 2023-2024 thuong.nv <thuong.nv.mta@gmail.com>
* Distributed under the MIT software Licencs, see the accompanying
* File COPYING or http://www.opensource.org/licenses/mit-license.php
*
* @file     console_if.h
* @create   june 23, 2024
* @brief    Console if
************************************************************************************/

#ifndef CONSOLE_IF_H
#define CONSOLE_IF_H

#include "console_type.h"

interface ConsolePlatform
{
	virtual void* GetHandle() noexcept = 0;
};

#endif // CONSOLE_IF_H