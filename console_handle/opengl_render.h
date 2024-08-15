////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************
* Copyright (C) 2023-2024 thuong.nv <thuong.nv.mta@gmail.com>
* MIT software Licencs, see the accompanying
* http://www.opensource.org/licenses/mit-license.php
*
/***********************************************************************************
* @brief : Render use OpenGL
* @file  : opengl_render.h
* @create: Aug 18, 2024
* @note  : For conditions of distribution and use, see copyright notice in readme.txt
***********************************************************************************/

#pragma once
#include "console_device.h"
#include <Windows.h>
#include "GL/glew.h"
#include "GL/wglew.h"
#include <assert.h>
#include "xopenglctx.h"
#include "console_handle.h"
#include <memory>

/////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************/
// IConsoleObjectRender interface

interface IConsoleObjectRender
{
protected:
	DeviceContextPtr m_pContext{ nullptr };

public:
	virtual void SetContext(DeviceContextPtr pContext) { m_pContext = pContext; }
	virtual void SetView(ConsoleViewPtr* pView) = 0;

	virtual void Draw() = 0;
	virtual void Clear() = 0;
};

/////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************/
// OpenGLConsoleShapeRender class
class OpenGLConsoleShapeRender : public IConsoleObjectRender
{
	enum
	{
		RESERVE_BUFF_DATA = 1000,
	};

public:
	OpenGLConsoleShapeRender()
	{
		m_vecLineData.reserve(RESERVE_BUFF_DATA);
		m_vecRectData.reserve(RESERVE_BUFF_DATA);
	}

	~OpenGLConsoleShapeRender()
	{

	}

public:
	bool CheckData()
	{
		return true;
	}

public:
	virtual void AddLine(const ConsolePoint& pt1, const ConsolePoint& pt2,
						 const ConsoleColor& cl1, const ConsoleColor& cl2) noexcept
	{
		if (!CheckData())
			return;

		// 24 * sizeof(float)
		m_vecLineData.push_back(pt1.x);
		m_vecLineData.push_back(pt1.y);
		m_vecLineData.push_back(pt1.z);

		m_vecLineData.push_back(cl1.r);
		m_vecLineData.push_back(cl1.g);
		m_vecLineData.push_back(cl1.b);

		m_vecLineData.push_back(pt2.x);
		m_vecLineData.push_back(pt2.y);
		m_vecLineData.push_back(pt2.z);

		m_vecLineData.push_back(cl2.r);
		m_vecLineData.push_back(cl2.g);
		m_vecLineData.push_back(cl2.b);
	}

	virtual void AddLine(const ConsolePoint& pt1, const ConsolePoint& pt2, const float& fz,
						 const ConsoleColor& cl1, const ConsoleColor& cl2) noexcept
	{
		if (!CheckData())
			return;

		// 24 * sizeof(float)
		m_vecLineData.push_back(pt1.x);
		m_vecLineData.push_back(pt1.y);
		m_vecLineData.push_back(fz);

		m_vecLineData.push_back(cl1.r / 255.f);
		m_vecLineData.push_back(cl1.g / 255.f);
		m_vecLineData.push_back(cl1.b / 255.f);

		m_vecLineData.push_back(pt2.x);
		m_vecLineData.push_back(pt2.y);
		m_vecLineData.push_back(fz);

		m_vecLineData.push_back(cl2.r / 255.f);
		m_vecLineData.push_back(cl2.g / 255.f);
		m_vecLineData.push_back(cl2.b / 255.f);
	}

	virtual void AddRect(const ConsolePoint& pt1, const float& fWidth, const float& fHeight,
						 const ConsoleColor& clr) noexcept
	{
		if (!CheckData())
			return;

		m_vecRectData.push_back(pt1.x);
		m_vecRectData.push_back(pt1.y);
		m_vecRectData.push_back(pt1.z);

		m_vecRectData.push_back(clr.r / 255.f);
		m_vecRectData.push_back(clr.g / 255.f);
		m_vecRectData.push_back(clr.b / 255.f);

		m_vecRectData.push_back(pt1.x + fWidth);
		m_vecRectData.push_back(pt1.y);
		m_vecRectData.push_back(pt1.z);

		m_vecRectData.push_back(clr.r / 255.f);
		m_vecRectData.push_back(clr.g / 255.f);
		m_vecRectData.push_back(clr.b / 255.f);

		m_vecRectData.push_back(pt1.x + fWidth);
		m_vecRectData.push_back(pt1.y + fHeight);
		m_vecRectData.push_back(pt1.z);

		m_vecRectData.push_back(clr.r / 255.f);
		m_vecRectData.push_back(clr.g / 255.f);
		m_vecRectData.push_back(clr.b / 255.f);

		m_vecRectData.push_back(pt1.x);
		m_vecRectData.push_back(pt1.y + fHeight);
		m_vecRectData.push_back(pt1.z);

		m_vecRectData.push_back(clr.r / 255.f);
		m_vecRectData.push_back(clr.g / 255.f);
		m_vecRectData.push_back(clr.b / 255.f);
	}

	virtual void AddRect(const ConsolePoint& pt1, const float& fZ, const float& fWidth, const float& fHeight,
						 const ConsoleColor& clr) noexcept
	{
		if (!CheckData())
			return;

		m_vecRectData.push_back(pt1.x);
		m_vecRectData.push_back(pt1.y);
		m_vecRectData.push_back(fZ);

		m_vecRectData.push_back(clr.r);
		m_vecRectData.push_back(clr.g);
		m_vecRectData.push_back(clr.b);

		m_vecRectData.push_back(pt1.x + fWidth);
		m_vecRectData.push_back(pt1.y);
		m_vecRectData.push_back(fZ);

		m_vecRectData.push_back(clr.r);
		m_vecRectData.push_back(clr.g);
		m_vecRectData.push_back(clr.b);

		m_vecRectData.push_back(pt1.x + fWidth);
		m_vecRectData.push_back(pt1.y + fHeight);
		m_vecRectData.push_back(fZ);

		m_vecRectData.push_back(clr.r);
		m_vecRectData.push_back(clr.g);
		m_vecRectData.push_back(clr.b);

		m_vecRectData.push_back(pt1.x);
		m_vecRectData.push_back(pt1.y + fHeight);
		m_vecRectData.push_back(fZ);

		m_vecRectData.push_back(clr.r);
		m_vecRectData.push_back(clr.g);
		m_vecRectData.push_back(clr.b);
	}

public:
	virtual void SetView(ConsoleView* pView)
	{

	}

	virtual void Draw()
	{
		int nRectLength = static_cast<int>(m_vecRectData.size());
		if (nRectLength > 0)
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			glVertexPointer(3, GL_FLOAT, 6 * sizeof(float), &m_vecRectData[0]);
			glColorPointer(3, GL_FLOAT, 6 * sizeof(float), &m_vecRectData[3]);
			glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(nRectLength / 6));
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);
		}

		size_t szLineLength = static_cast<int>(m_vecLineData.size());
		if (szLineLength > 0)
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			glVertexPointer(3, GL_FLOAT, 6 * sizeof(float), &m_vecLineData[0]);
			glColorPointer(3, GL_FLOAT, 6 * sizeof(float), &m_vecLineData[3]);
			glDrawArrays(GL_LINES, 0, (GLsizei)(szLineLength / 6));
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);
		}
	}

	virtual void Clear()
	{
		m_vecLineData.clear();
		m_vecRectData.clear();
	}

protected:
	std::vector<float> m_vecLineData;
	std::vector<float> m_vecRectData;
};


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

protected:
	void Init()
	{
		// Create list all charactor vietnamese
		m_nTextList = glGenLists(rang_base_list);
	}

public:
	OpenGLConsoleTextRender()
	{
		Init();
	}

	~OpenGLConsoleTextRender()
	{
		if (m_nTextList)
		{
			glDeleteLists(m_nTextList, rang_base_list);
		}

		if (m_nBaseList)
		{
			glDeleteLists(m_nBaseList, 1);
		}
	}

	void AddText(const ConsolePoint& pt, const ConsoleColor& color, const ConsoleString& str)
	{
		m_vecData.push_back({pt, color, str});
	}

	void AddText(const ConsolePoint& pt, const float& fZ, const ConsoleColor& color, const ConsoleString& str)
	{
		m_vecData.push_back({ ConsolePoint{pt.x, pt.y, fZ}, color, str });
	}

	void Draw()
	{
		if (MakeRenderContext())
		{
			for (size_t i = 0; i < m_vecData.size(); i++)
			{
				glColor4f(m_vecData[i].color.r, m_vecData[i].color.g, m_vecData[i].color.b, 1.f);
				glRasterPos3f(m_vecData[i].pt.x, m_vecData[i].pt.y, m_vecData[i].pt.z);
				glCallLists((GLsizei)m_vecData[i].str.length(), GL_UNSIGNED_SHORT, m_vecData[i].str.c_str());
			}
		}
	}

protected:
	bool MakeRenderContext()
	{
		if (!m_pContext || !m_pContext->MakeCurrentContext())
			return false;

		HDC hDC = static_cast<HDC>(m_pContext->Render());
		HFONT hFont = static_cast<HFONT>(m_pFont->GetHandle());

		if (!hDC || !hFont)
			return false;

		// Select a device context for the font
		SelectObject(hDC, hFont);

		// Only initialized once
		if (!m_nBaseList)
		{
			m_nBaseList = glGenLists(1);

			glNewList(m_nBaseList, GL_COMPILE);
			{
				glListBase(m_nTextList - 32);

				// Push information matrix
				glPushAttrib(GL_LIST_BIT);

				// Load model view matrix
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glLoadIdentity();

				// Load projection matrix + can use glm;
				glMatrixMode(GL_PROJECTION);
				glPushMatrix();
				glLoadIdentity();

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glDisable(GL_DEPTH_TEST);
			}
			glEndList();

			if (!wglUseFontBitmaps(hDC, 32, rang_base_list, m_nTextList))
			{
				assert(0);
				return false;
			}
		}

		glCallList(m_nBaseList);
		gluOrtho2D(0.0, m_nWidth, m_nHeight, 0.0);

		return true;
	}

public:
	void SetView(const int nWidth, const int nHeight) noexcept
	{
		m_nWidth = nWidth;
		m_nHeight = nHeight;
	}

	void SetFont(ConsoleFont* pFont) noexcept
	{
		m_pFont = pFont;
	}

	ConsoleFont* GetFont() const noexcept { return m_pFont; }

	void SetContext(DeviceContext* pContext) noexcept
	{
		m_pContext = pContext;
	}

protected:
	int					m_nWidth{ 0 };
	int					m_nHeight{ 0 };
	const int			rang_base_list = 9000;
	ConsoleFont*		m_pFont{ nullptr };
	DeviceContext*		m_pContext{ nullptr };

	GLuint				m_nTextList{ 0 };
	GLuint				m_nBaseList{ 0 };

	std::vector<TextRenderData> m_vecData;
};

/******************************************************************************/
/*OpenGLConsoleDevice*/

class OpenGLConsoleDevice : public ConsoleDevice
{
	enum { MAX_RENDER_DATA = 10000 };

	using OpenGLConsoleTextRenderPtr = std::shared_ptr<OpenGLConsoleTextRender>;
	using OpenGLConsoleShapeRenderPtr = std::shared_ptr<OpenGLConsoleShapeRender>;
	using DeviceContextPtr = std::shared_ptr<DeviceContext>;

public:
	OpenGLConsoleDevice(DeviceContextConfig config)
	{
		m_pContext = std::make_shared<OpenGLDeviceContext>(config);
		m_pBoardRender = std::make_shared<OpenGLConsoleShapeRender>();
		m_pCustomRender = std::make_shared<OpenGLConsoleShapeRender>();
		m_pFontManager = std::make_shared<ConsoleFontManager>();
	}

	~OpenGLConsoleDevice()
	{
		m_pContext->DeleteContext();
	}

public:
	bool CreateDeviceContext(ConsoleHandle* pHandle)
	{
		HWND hWnd = static_cast<HWND>(pHandle->GetHandle());

		bool bCreateDone = m_pContext->CreateContext(hWnd);

		m_pDeviceCtrl->AddFlags(DEVICEIP_UPDATE_BOARD | DEVICEIP_UPDATE_COORD);

		return bCreateDone;
	}

protected:
	void UpdateCoord(ConsoleView* pView)
	{
		if (!pView)
			return;

		glEnable(GL_DEPTH_TEST);

		int nWidth = pView->GetWidth();
		int nHeight = pView->GetHeight();

		glViewport(0, 0, pView->GetWidth(), pView->GetHeight());

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		auto eCoordType = pView->GetCoordType();

		// Sample set left-top coordinates matrix
		if (eCoordType == TopLeft)
		{
			GLdouble left = -1.f;
			GLdouble right = GLdouble(nWidth);
			GLdouble top = -1.f;
			GLdouble bottom = GLdouble(nHeight);

			glOrtho(left, right, bottom, top, GLdouble(0.0), GLdouble(-1000.0));
		}
		// Sample set center coordinates matrix
		else if (eCoordType == Center)
		{
			GLdouble left = -GLdouble(nWidth) / 2.f;
			GLdouble right = GLdouble(nWidth) / 2.f;
			GLdouble top = -GLdouble(nHeight) / 2.f;
			GLdouble bottom = GLdouble(nHeight) / 2.f;

			glOrtho(left, right, bottom, top, GLdouble(0.0), GLdouble(1000.0));

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			glRotatef(180, 1, 0, 0);
		}
	}

	ConsoleFont* GetFont(const wchar_t* font_name, const int font_size)
	{
		if (m_pFontManager)
		{
			auto spFont = m_pFontManager->Get(font_name, font_size);
			return (spFont) ? spFont.get() : nullptr;
		}

		return nullptr;
	}

public:

	virtual bool Begin(ConsoleView* pView)
	{
		if (!pView || !m_pContext)
			return false;

		if (m_pContext->MakeCurrentContext())
		{
			m_pGraphics = pView->GetGraphics();

			if (m_pDeviceCtrl->CheckFlags(DEVICEIP_UPDATE_COORD))
				UpdateCoord(pView);

			m_pDeviceCtrl->AddFlags(DEVICEIP_UPDATE_CUR);

			HDC* pHDC = reinterpret_cast<HDC*>(m_pContext->Render());


			//if (m_hFont)
			//	::DeleteObject(m_hFont);

			//m_hFont = ::CreateFont(12, 0, 0, 0, 400, FALSE, FALSE, FALSE,
			//	DEFAULT_CHARSET, OUT_TT_PRECIS,
			//	CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
			//	FF_DONTCARE | DEFAULT_PITCH, _T("Consolas"));


			//m_nTextList = glGenLists(128);

			//// Select a device context for the font
			//auto oldFont = ::SelectObject(*pHDC, m_hFont);

			//if (!m_nBaseList)
			//{
			//	m_nBaseList = glGenLists(1);

			//	glNewList(m_nBaseList, GL_COMPILE);
			//	{
			//		glListBase(m_nTextList - 32);

			//		// Push information matrix
			//		glPushAttrib(GL_LIST_BIT);

			//		// Load model view matrix
			//		glMatrixMode(GL_MODELVIEW);
			//		glPushMatrix();
			//		glLoadIdentity();

			//		// Load projection matrix + can use glm;
			//		glMatrixMode(GL_PROJECTION);
			//		glPushMatrix();
			//		glLoadIdentity();

			//		glEnable(GL_BLEND);
			//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			//		glDisable(GL_DEPTH_TEST);
			//	}
			//	glEndList();

			//	if (!wglUseFontBitmaps(*pHDC, 32, 128, m_nTextList))
			//	{
			//		assert(0);
			//		return false;
			//	}
			//}

			//::SelectObject(*pHDC, oldFont);

			glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			return true;
		}

		return false;
	}

	virtual void End()
	{
		if (m_pContext)
			m_pContext->SwapBuffer();

		m_pDeviceCtrl->ClearFlags();
	}

	virtual void Draw()
	{
		/*Draw board*/
		m_pBoardRender->Draw();


		/*Draw custom */
		glBegin(GL_LINES);
		{
			glLineWidth(5.f);

			glColor3f(0.f, 1.f, 0);

			glVertex3f(0.f, -20.f, 0.0);
			glVertex3f(0.f, 20.f, 0.0);

			glVertex3f(-20.f, 0.f, 0.0);
			glVertex3f(20.f, 0.f, 0.0);
		}
		glEnd();

		float a = -1.f;

		glBegin(GL_LINES);
		{
			glLineWidth(5.f);

			glColor3f(1.f, 0.f, 0);

			glVertex3f(0.f, -40.f, a);
			glVertex3f(0.f, 40.f, a);

			glVertex3f(-40.f, 0.f, a);
			glVertex3f(40.f, 0.f, a);
		}
		glEnd();

		m_pCustomRender->Draw();
	}

	virtual void Update()
	{
		if (m_pDeviceCtrl->CheckFlags(DEVICEIP_UPDATE_BOARD))
		{
			m_pBoardRender->Clear();

			UpdateBoardRenderData();
		}

		if (m_pDeviceCtrl->CheckFlags(DEVICEIP_UPDATE_CUR))
		{
			m_pCustomRender->Clear();

			UpdateCustomRenderData();
			UpdateTextRenderData();
		}
	}

	virtual void Clear()
	{
		m_fZCurrent = m_fZMaxBoard;
		m_pCustomRender->Clear();
	}

	virtual void ClearBoard()
	{
		m_fZCurrent = 0.1f;
		m_pBoardRender->Clear();
	}

protected:
	
	virtual void UpdateShapeRenderData(OpenGLConsoleShapeRenderPtr pShapeRender, ConsoleDrawBuffer* pDrawBuffer)
	{
		if (!pDrawBuffer)
			return;

		if (!pShapeRender || !pShapeRender.get())
			return;

		float fZReal = m_fZMax - m_fZCurrent;
		float fZ = 0.f;

		// Push data line render
		auto Lines = pDrawBuffer->GetLinesDrawBuffer();
		size_t nLineBufferCnt = Lines.size();

		if (nLineBufferCnt > 0)
		{
			for (auto i = 0; i < nLineBufferCnt; i++)
			{
				fZ = fZReal - Lines[i].first;
				pShapeRender->AddLine(Lines[i].second.pt1, Lines[i].second.pt2, fZ,
					Lines[i].second.col, Lines[i].second.col);

				if (fZ < fZReal)
					m_fZCurrent = m_fZMax - fZ;
			}
		}

		// Push data rectangle render
		auto Rects = pDrawBuffer->GetRectsDrawBuffer();
		size_t nRectBufferCnt = Rects.size();

		if (nRectBufferCnt > 0)
		{
			for (auto i = 0; i < nRectBufferCnt; i++)
			{
				fZ = fZReal - Rects[i].first;
				pShapeRender->AddRect(Rects[i].second.pt, fZ,
					Rects[i].second.width, Rects[i].second.height, Rects[i].second.col);

				if (fZ < fZReal)
					m_fZCurrent = m_fZMax - fZ;
			}
		}
	}

	/*Board Data*/
	virtual void UpdateBoardRenderData()
	{
		if (!m_pGraphics)
			return;

		ConsoleDrawBuffer* pBoardDrawBuffer = m_pGraphics->GetBoardBufferData();
		UpdateShapeRenderData(m_pBoardRender, pBoardDrawBuffer);

		m_fZMaxBoard = m_fZCurrent;
	}

	/*Custom Data*/
	virtual void UpdateCustomRenderData()
	{
		if (!m_pGraphics)
			return;

		m_fZCurrent = m_fZMaxBoard;

		ConsoleDrawBuffer* pBoardDrawBuffer = m_pGraphics->GetBufferData();
		UpdateShapeRenderData(m_pCustomRender, pBoardDrawBuffer);

		m_fZMaxCustom = m_fZCurrent;
	}

	/*Text Data*/
	virtual void UpdateTextRenderData()
	{
		if (!m_pGraphics)
			return;

		auto Texts = m_pGraphics->GetBufferData()->GetTextsDrawBuffer();

		if (Texts.empty())
			return;

		OpenGLConsoleTextRenderPtr pFontRender = nullptr;
		ConsoleDrawBuffer::VEC_TEXT_DRAW_DATA* pVecTextDrawData = nullptr;
		ConsoleDrawBuffer::TEXT_DRAW* pTextDraw = nullptr;

		float fZReal = m_fZMax - m_fZCurrent;
		float fZ = 0.f;

		for (auto it = Texts.begin(); it != Texts.end(); it++)
		{
			auto pFont = m_pFontManager->Get(it->first);

			if (!pFont) continue;

			pVecTextDrawData = &it->second;

			auto itFontRender = m_FontRender.find(pFont.get());

			if (itFontRender != m_FontRender.end())
			{
				pFontRender = itFontRender->second;

				for (int i = 0; pVecTextDrawData->size(); i++)
				{
					pTextDraw = &pVecTextDrawData->at(i).second;
					fZ = fZReal + pVecTextDrawData->at(i).first;

					pFontRender->AddText(pTextDraw->pt, fZ, pTextDraw->col, pTextDraw->str);
				}
			}
		}
	}

	template<class T> struct ptr_less
	{
		bool operator()(T* lhs, T* rhs)
		{
			return *lhs < *rhs;
		}
	};

protected:
	float m_fZCurrent = 0.1f;
	float m_fZMaxCustom = 0.1f;
	float m_fZMaxBoard = 0.1f;
	const float m_fZMax = 900.f;

	OpenGLConsoleShapeRenderPtr m_pBoardRender;
	OpenGLConsoleShapeRenderPtr m_pCustomRender;

	std::map<ConsoleFont*, OpenGLConsoleTextRenderPtr> m_FontRender;

	ConsoleGraphics*      m_pGraphics{ nullptr };
	DeviceContextPtr      m_pContext{ nullptr };
	ConsoleFontManagerPtr m_pFontManager{ nullptr };

	GLuint				m_nTextList{ 0 };
	GLuint				m_nBaseList{ 0 };
	HFONT			m_hFont{ NULL };
};