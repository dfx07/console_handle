////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************
* Copyright (C) 2023-2024 thuong.nv <thuong.nv.mta@gmail.com>
* MIT software Licencs, see the accompanying
* http://www.opensource.org/licenses/mit-license.php
*
/***********************************************************************************
* @brief : Define opengl render
* @file  : gl_render_def.h
* @create: Aug 16, 2024
* @note  : For conditions of distribution and use, see copyright notice in readme.txt
***********************************************************************************/

#ifndef GL_RENDER_DEF_H
#define GL_RENDER_DEF_H

#include "xctx.h"
#include "console_type.h"

#ifndef interface
#define interface struct
#endif // !interface

/////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************/
// IConsoleObjectRender interface

interface IConsoleObjectRender
{
protected:
	DeviceContextPtr m_pContext{ nullptr };

public:
	virtual void SetContext(DeviceContextPtr pContext) noexcept { m_pContext = pContext; }
	virtual void SetView(const float fWidth, const float fHeight) noexcept = 0;

	virtual void Draw() = 0;
	virtual void Clear() = 0;
};

class OpenGLConsoleShapeRender;
typedef std::shared_ptr<OpenGLConsoleShapeRender> OpenGLConsoleShapeRenderPtr;

class OpenGLConsoleTextRender;
typedef std::shared_ptr<OpenGLConsoleTextRender> OpenGLConsoleTextRenderPtr;


#endif // !GL_RENDER_DEF_H