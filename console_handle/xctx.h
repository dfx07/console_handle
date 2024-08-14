////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************
* Copyright (C) 2023-2024 thuong.nv <thuong.nv.mta@gmail.com>
* MIT software Licencs, see the accompanying
* http://www.opensource.org/licenses/mit-license.php
*
/***********************************************************************************
* @brief : Common context define
* @file  : xcmctx.h
* @create: Aug 14, 2024
* @note  : For conditions of distribution and use, see copyright notice in readme.txt
************************************************************************************/

#ifndef XCTX_H
#define XCTX_H

#define interface struct

interface IContext
{
	virtual bool CreateContext(void* hHandle) = 0;
	virtual void DeleteContext() noexcept = 0;
	virtual bool MakeCurrentContext() const noexcept = 0;
	virtual void SwapBuffer() const noexcept = 0;
};

interface IDeviceContext : public IContext
{
	virtual void* Render() const noexcept = 0;
	virtual bool  IsValid() const noexcept = 0;
};

#endif // !XCTX_H