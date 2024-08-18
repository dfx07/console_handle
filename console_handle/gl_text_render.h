////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************
* Copyright (C) 2023-2024 thuong.nv <thuong.nv.mta@gmail.com>
* MIT software Licencs, see the accompanying
* http://www.opensource.org/licenses/mit-license.php
*
/***********************************************************************************
* @brief : Text rendering use opengl
* @file  : gl_text_render.h
* @create: Aug 18, 2024
* @note  : For conditions of distribution and use, see copyright notice in readme.txt
***********************************************************************************/

#ifndef GL_TEXT_RENDER_H
#define GL_TEXT_RENDER_H

#define IS_OPENGL_ZERO(p) (p == GL_ZERO)

#include <vector>
#include "GL/glew.h"
#include "GL/wglew.h"
#include "gl_render_def.h"

#include "console_font.h"

//#pragma comment (lib,"glu32.lib")

/////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************/
// OpenGLConsoleShapeRender class
/*
Display 2D text use system font. Not support zoom
Feature:
	+ Support bitmap vietnamese
	+ Support special characters
Bitmap fonts offer a simple way to display 2D text on the screen.
Information about the characters in a bitmap font is stored as bitmap images.

[*] Advantage to bitmap fonts is that they provide a high performance method for render image text to the screen
[*] If you not use Unicode , please reduce RANG_BASE_LIST = asscii
*/
class OpenGLConsoleTextRender : public IConsoleObjectRender
{
	typedef struct {
		ConsolePoint	pt;
		ConsoleColor	color;
		ConsoleString	str;
	} TextRenderData;

	enum {
		ENUM_RANGE_BASE = 128,
	};

public:
	OpenGLConsoleTextRender()
	{

	}

	~OpenGLConsoleTextRender()
	{
		DeleteTextRender();
	}

public: // Data function

	void AddText(const ConsolePoint& pt, const ConsoleColor& color, const ConsoleString& str)
	{
		m_vecData.push_back({pt, color, str});
	}

	void AddText(const ConsolePoint& pt, const float& fZ, const ConsoleColor& color, const ConsoleString& str)
	{
		m_vecData.push_back({ ConsolePoint{pt.x, pt.y, fZ}, color, str });
	}

public:
	/********************************************************************************
	*! @brief  : Draw text data using opengl
	*! @return : void
	*! @author : thuong.nv          - [Date] : 2024.08.16
	*! @note   : MakeContext before drawing
	********************************************************************************/
	void Draw()
	{
		if (IsRenderable())
		{
			if (Use())
			{
				for (size_t i = 0; i < m_vecData.size(); i++)
				{
					glColor4f(m_vecData[i].color.r, m_vecData[i].color.g, m_vecData[i].color.b, 1.f);
					glRasterPos3f(m_vecData[i].pt.x, m_vecData[i].pt.y, m_vecData[i].pt.z);
					glCallLists((GLsizei)m_vecData[i].str.length(), GL_UNSIGNED_SHORT, m_vecData[i].str.c_str());
				}
				UnUse();
			}
		}
	}

	/********************************************************************************
	*! @brief  : Check condition to drawing (context and make context)
	*! @return : bool  true / false
	*! @author : thuong.nv          - [Date] : 2024.08.16
	********************************************************************************/
	bool IsRenderable() const noexcept
	{
		if (!m_pContext || !m_pContext->MakeCurrentContext())
			return false;
		return true;
	}

protected:

	/********************************************************************************
	*! @brief  : Delete opengl text render
	*! @return : void
	*! @author : thuong.nv          - [Date] : 2024.08.16
	********************************************************************************/
	void DeleteTextRender()
	{
		if (m_nTextList)
			glDeleteLists(m_nTextList, ENUM_RANGE_BASE);

		m_nTextList = 0;

		if (m_nBaseList)
			glDeleteLists(m_nBaseList, 1);

		m_nBaseList = 0;
	}

	/********************************************************************************
	*! @brief  : Create bitmap text
	*! @param  : [In] bForce : force create
	*! @return : bool  true / false
	*! @author : thuong.nv          - [Date] : 2024.08.16
	********************************************************************************/
	bool CreateBitmapText(bool bForce = false)
	{
		HDC* pHDC = reinterpret_cast<HDC*>(m_pContext->Render());
		HFONT* pHFont = reinterpret_cast<HFONT*>(m_pFont->GetHandle());

		if (!pHDC || !pHFont)
			return false;

		HDC hDC = *pHDC; HFONT hFont = *pHFont;

		if (bForce) DeleteTextRender();

		// Select a device context for the font
		auto bOldHFont = ::SelectObject(hDC, hFont);

		// Only initialized once
		if (IS_OPENGL_ZERO(m_nBaseList))
		{
			m_nTextList = glGenLists(ENUM_RANGE_BASE);
			m_nBaseList = glGenLists(1);

			glNewList(m_nBaseList, GL_COMPILE);
			{
				glListBase(m_nTextList - 32);

				//// Push information matrix
				//glPushAttrib(GL_LIST_BIT);

				//// Load model view matrix
				//glMatrixMode(GL_MODELVIEW);
				//glPushMatrix();
				//glLoadIdentity();

				//// Load projection matrix + can use glm
				//glMatrixMode(GL_PROJECTION);
				//glPushMatrix();
				//glLoadIdentity();

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glDisable(GL_DEPTH_TEST);
			}
			glEndList();

			if (!wglUseFontBitmaps(hDC, 32, ENUM_RANGE_BASE, m_nTextList))
			{
				assert(0);
				::SelectObject(hDC, bOldHFont);
				return false;
			}
		}

		::SelectObject(hDC, bOldHFont);

		return true;
	}

	/********************************************************************************
	*! @brief  : Use context to draw text -Render 2D (must use)
	*! @return : void
	*! @author : thuong.nv          - [Date] : 2024.08.16
	********************************************************************************/
	bool Use()
	{
		if (!m_pRenderCtrl || !m_pRenderCtrl->GetViewInfo())
			return false;

		auto pViewInfo = m_pRenderCtrl->GetViewInfo();

		if (IS_OPENGL_ZERO(m_nBaseList))
		{
			if (!CreateBitmapText())
				return false;
		}

		int nWidth = static_cast<int>(pViewInfo->GetWidth());
		int nHeight = static_cast<int>(pViewInfo->GetHeight());
		auto eCoordType = pViewInfo->GetCoordType();

		glViewport(0, 0, nWidth, nHeight);

		//glPushAttrib(GL_LIST_BIT);

		// Load projection matrix
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();

		// Sample set left-top coordinates matrix
		if (eCoordType == TopLeft)
		{
			GLdouble left   = -1.f;
			GLdouble right  = GLdouble(nWidth);
			GLdouble top    = -1.f;
			GLdouble bottom = GLdouble(nHeight);

			glOrtho(left, right, bottom, top, GLdouble(0.0), GLdouble(-1000.0));
		}
		// Sample set center coordinates matrix
		else if (eCoordType == Center)
		{
			GLdouble left   = -GLdouble(nWidth) / 2.f;
			GLdouble right  =  GLdouble(nWidth) / 2.f;
			GLdouble top    =  GLdouble(nHeight)/ 2.f;
			GLdouble bottom =  GLdouble(nHeight)/ 2.f;

			glOrtho(left, right, bottom, top, GLdouble(0.0), GLdouble(1000.0));
		}

		// Load model-view matrix 
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		if (eCoordType == Center)
		{
			glRotatef(180, 1, 0, 0);
		}

		glCallList(m_nBaseList);

		return true;
	}

	/********************************************************************************
	*! @brief  : Pop info after drawing finish (must use)
	*! @return : void
	*! @author : thuong.nv          - [Date] : 2024.08.16
	********************************************************************************/
	void UnUse()
	{
		// Pop projection matrix + can use glm
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		// Pop model view matrix
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		glListBase(0);
		// Pop information matrix
		glPopAttrib();
	}

public:

	/********************************************************************************
	*! @brief  : Set draw context
	*! @param  : [In] pContext : context pointer
	*! @return : void
	*! @author : thuong.nv          - [Date] : 2024.08.17
	********************************************************************************/
	virtual void SetContext(DeviceContextPtr pContext) noexcept override
	{
		if (m_pContext != pContext)
		{
			m_pContext = pContext;
		}
	}

	virtual void Clear() override
	{
		m_vecData.clear();
	}

	void SetFont(ConsoleFontPtr pFont) noexcept { m_pFont = pFont; }
	ConsoleFontPtr GetFont() const noexcept { return m_pFont; }

protected:
	ConsoleFontPtr		m_pFont{ nullptr };
	DeviceContextPtr	m_pContext{ nullptr };

	GLuint				m_nTextList{ GL_ZERO };
	GLuint				m_nBaseList{ GL_ZERO };

	std::vector<TextRenderData> m_vecData;
};


#endif // !GL_TEXT_RENDER_H