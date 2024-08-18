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

class ConsoleRenderControl;
typedef std::shared_ptr<ConsoleRenderControl> ConsoleRenderControlPtr;

interface IConsoleObjectRender
{
	virtual void SetContext(DeviceContextPtr pContext) noexcept { m_pContext = pContext; }

	virtual void Draw() = 0;
	virtual void Clear() = 0;

	virtual void SetRenderControl(ConsoleRenderControlPtr pControl) {
		m_pRenderCtrl = pControl;
	}

protected:
	DeviceContextPtr			m_pContext{ nullptr };
	ConsoleRenderControlPtr		m_pRenderCtrl{ nullptr };
};

typedef struct _ConsoleViewInfo {
	float			 m_fWidth;
	float			 m_fHeight;
	ConsoleViewCoord m_ViewCoord;

public:
	const float GetWidth() const noexcept { return m_fWidth; }
	const float GetHeight() const noexcept { return m_fHeight; }
	const ConsoleViewCoord GetCoordType() const noexcept { return m_ViewCoord; }

public:
	void SetView(const float fwidth, const float fheight) noexcept
	{
		m_fWidth = fwidth;
		m_fHeight = fheight;
	}

	void SetViewCoord(ConsoleViewCoord viewcoord) noexcept
	{
		m_ViewCoord = viewcoord;
	}

}ConsoleViewInfo;

typedef std::shared_ptr<ConsoleViewInfo> ConsoleViewInfoPtr;

/////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************/
// ConsoleRenderControl interface

class ConsoleRenderControl
{
public:
	void SetViewInfo(ConsoleViewInfoPtr pViewInfo) noexcept
	{
		m_pViewInfo = pViewInfo;
	}

	ConsoleViewInfoPtr GetViewInfo() const noexcept
	{
		return m_pViewInfo;
	}

public:
	ConsoleViewInfoPtr m_pViewInfo;
};

class OpenGLConsoleShapeRender;
typedef std::shared_ptr<OpenGLConsoleShapeRender> OpenGLConsoleShapeRenderPtr;

class OpenGLConsoleTextRender;
typedef std::shared_ptr<OpenGLConsoleTextRender> OpenGLConsoleTextRenderPtr;


#endif // !GL_RENDER_DEF_H