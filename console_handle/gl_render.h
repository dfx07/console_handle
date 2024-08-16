////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************
* Copyright (C) 2023-2024 thuong.nv <thuong.nv.mta@gmail.com>
* MIT software Licencs, see the accompanying
* http://www.opensource.org/licenses/mit-license.php
*
/***********************************************************************************
* @brief : Render use OpenGL
* @file  : gl_render.h
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

#include "gl_shape_render.h"
#include "gl_text_render.h"


/////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************/
// OpenGLConsoleDevice class

class OpenGLConsoleDevice : public ConsoleDevice
{
	enum { MAX_RENDER_DATA = 10000 };

public:

	OpenGLConsoleDevice(DeviceContextConfig& config)
	{
		InitData(config);
	}

	OpenGLConsoleDevice(bool bUseOpenGlExt = false)
	{
		DeviceContextConfig _config;
		_config.UseOpenGLExtension(true);

		InitData(_config);
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

	void InitData(DeviceContextConfig& config)
	{
		m_pContext = std::make_shared<OpenGLDeviceContext>(config);
		m_pBoardRender = std::make_shared<OpenGLConsoleShapeRender>();
		m_pCustomRender = std::make_shared<OpenGLConsoleShapeRender>();
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

public:

	virtual bool Begin(ConsoleView* pView)
	{
		if (!pView || !m_pContext)
			return false;

		m_fWidthView = static_cast<float>(pView->GetWidth());
		m_fHeightView = static_cast<float>(pView->GetHeight());
		m_pGraphics = pView->GetGraphics();

		if (m_pContext->MakeCurrentContext())
		{
			for (auto& pRenderText : m_TextRender)
			{
				pRenderText.second->SetContext(m_pContext);
			}

			if (m_pDeviceCtrl->ValidFlags(DEVICEIP_UPDATE_COORD))
				UpdateCoord(pView);

			m_pDeviceCtrl->AddFlags(DEVICEIP_UPDATE_CUR);

			//HDC* pHDC = reinterpret_cast<HDC*>(m_pContext->Render());


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

	virtual void End() override
	{
		if (m_pContext)
			m_pContext->SwapBuffer();

		m_pDeviceCtrl->ClearFlags();
	}

	virtual void Draw()
	{
		/*Draw board*/
		m_pBoardRender->Draw();

		/*Draw text*/
		for (auto& pRenderText : m_TextRender)
		{
			pRenderText.second->Draw();
		}

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
		if (m_pDeviceCtrl->ValidFlags(DEVICEIP_UPDATE_BOARD))
		{
			m_pBoardRender->Clear();

			UpdateBoardRenderData();
		}

		if (m_pDeviceCtrl->ValidFlags(DEVICEIP_UPDATE_CUR))
		{
			m_pCustomRender->Clear();

			for (auto& pRenderText : m_TextRender)
			{
				pRenderText.second->Clear();
			}

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

		ConsoleFontManagerPtr pFontManager = m_pDeviceCtrl->GetFontManager();

		if (pFontManager == nullptr)
			return;

		ConsoleFontPtr pDefaultFont = pFontManager->GetDefaultFont();
		if (pDefaultFont == nullptr)
			return;

		for (auto it = Texts.begin(); it != Texts.end(); it++)
		{
			ConsoleFontPtr pFont = pDefaultFont;

			if (!it->first.IsEmpty())
			{
				pFont = pFontManager->Get(it->first);
			}
			else
			{
				pFont = pDefaultFont;
			}

			if (!pFont || !pFont.get()) continue;

			auto itFontRender = m_TextRender.find(pFont.get());

			if (itFontRender != m_TextRender.end())
			{
				pFontRender = itFontRender->second;
			}
			else
			{
				pFontRender = std::make_shared<OpenGLConsoleTextRender>();
				pFontRender->SetFont(pFont);
				pFontRender->SetView(m_fWidthView, m_fHeightView);
				pFontRender->SetContext(m_pContext);

				m_TextRender.insert(std::make_pair(pFont.get(), pFontRender));
			}

			if (pFontRender == nullptr)
				continue;

			pVecTextDrawData = &it->second;
			for (size_t i = 0; i < pVecTextDrawData->size(); i++)
			{
				pTextDraw = &pVecTextDrawData->at(i).second;
				fZ = fZReal + pVecTextDrawData->at(i).first;

				pFontRender->AddText(pTextDraw->pt, fZ, pTextDraw->col / 255.f, pTextDraw->str);
			}
		}
	}

protected:
	float m_fZCurrent = 0.1f;
	float m_fZMaxCustom = 0.1f;
	float m_fZMaxBoard = 0.1f;
	const float m_fZMax = 900.f;

	float m_fWidthView{ 0.f };
	float m_fHeightView{ 0.f };

	OpenGLConsoleShapeRenderPtr m_pBoardRender;
	OpenGLConsoleShapeRenderPtr m_pCustomRender;

	std::map<ConsoleFont*, OpenGLConsoleTextRenderPtr> m_TextRender;

	ConsoleGraphics*      m_pGraphics{ nullptr };
	DeviceContextPtr      m_pContext{ nullptr };

	GLuint				m_nTextList{ 0 };
	GLuint				m_nBaseList{ 0 };
	HFONT				m_hFont{ NULL };
};