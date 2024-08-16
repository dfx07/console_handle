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

#include <vector>
#include "GL/glew.h"
#include "GL/wglew.h"
#include "gl_render_def.h"

#include "console_font.h"

#pragma comment (lib,"glu32.lib")

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
			Use();
			{
				for (size_t i = 0; i < m_vecData.size(); i++)
				{
					glColor4f(m_vecData[i].color.r, m_vecData[i].color.g, m_vecData[i].color.b, 1.f);
					glRasterPos3f(m_vecData[i].pt.x, m_vecData[i].pt.y, m_vecData[i].pt.z);
					glCallLists((GLsizei)m_vecData[i].str.length(), GL_UNSIGNED_SHORT, m_vecData[i].str.c_str());
				}
			}
			UnUse();
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
			glDeleteLists(m_nTextList, rang_base_list);

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
		if (!m_nBaseList)
		{
			m_nTextList = glGenLists(rang_base_list);

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

				//// Load projection matrix + can use glm;
				//glMatrixMode(GL_PROJECTION);
				//glPushMatrix();
				//glLoadIdentity();

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glDisable(GL_DEPTH_TEST);
			}
			glEndList();

			if (!wglUseFontBitmaps(hDC, 32, rang_base_list, m_nTextList))
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
		// Push information matrix
		glPushAttrib(GL_LIST_BIT);

		// Load model view matrix
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		// Load projection matrix + can use glm
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);

		glCallList(m_nBaseList);
		gluOrtho2D(0.0, (GLdouble)m_fWidth, (GLdouble)m_fHeight, 0.0);

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

		// Pop information matrix
		glPopAttrib();
	}

public:

	void SetView(const float fWidth, const float fHeight) noexcept
	{
		m_fWidth = fWidth;
		m_fHeight = fHeight;
	}

	virtual void Clear() override
	{
		m_vecData.clear();
	}

	void SetFont(ConsoleFontPtr pFont) noexcept { m_pFont = pFont; }
	ConsoleFontPtr GetFont() const noexcept { return m_pFont; }

protected:
	float				m_fWidth{ 0.f };
	float				m_fHeight{ 0.f };
	const int			rang_base_list = 9000;
	ConsoleFontPtr		m_pFont{ nullptr };
	DeviceContextPtr	m_pContext{ nullptr };

	GLuint				m_nTextList{ 0 };
	GLuint				m_nBaseList{ 0 };

	std::vector<TextRenderData> m_vecData;
};


#endif // !GL_TEXT_RENDER_H