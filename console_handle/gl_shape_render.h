////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************
* Copyright (C) 2023-2024 thuong.nv <thuong.nv.mta@gmail.com>
* MIT software Licencs, see the accompanying
* http://www.opensource.org/licenses/mit-license.php
*
/***********************************************************************************
* @brief : Shape rendering use opengl
* @file  : gl_shape_render.h
* @create: Aug 18, 2024
* @note  : For conditions of distribution and use, see copyright notice in readme.txt
***********************************************************************************/

#ifndef GL_SHAPE_RENDER_H
#define GL_SHAPE_RENDER_H

#include <vector>
#include <map>
#include "gl_render_def.h"
#include "GL/glew.h"

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
						 const ConsoleColor& cl1, const ConsoleColor& cl2,
						 const float fWidth = 1.f)
	{
		if (!CheckData())
			return;

		auto itFound = m_mapLineData.insert(std::make_pair(fWidth, std::vector<float>()));
		auto& vecLineData = itFound.first->second;

		if (itFound.second)
		{
			vecLineData.reserve(RESERVE_BUFF_DATA);
		}

		// 24 * sizeof(float)
		vecLineData.push_back(pt1.x);
		vecLineData.push_back(pt1.y);
		vecLineData.push_back(pt1.z);

		vecLineData.push_back(cl1.r / 255.f);
		vecLineData.push_back(cl1.g / 255.f);
		vecLineData.push_back(cl1.b / 255.f);

		vecLineData.push_back(pt2.x);
		vecLineData.push_back(pt2.y);
		vecLineData.push_back(pt2.z);

		vecLineData.push_back(cl2.r / 255.f);
		vecLineData.push_back(cl2.g / 255.f);
		vecLineData.push_back(cl2.b / 255.f);
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

		m_vecRectData.push_back(pt1.x);
		m_vecRectData.push_back(pt1.y + fHeight);
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

		m_vecRectData.push_back(pt1.x + fWidth);
		m_vecRectData.push_back(pt1.y);
		m_vecRectData.push_back(fZ);

		m_vecRectData.push_back(clr.r);
		m_vecRectData.push_back(clr.g);
		m_vecRectData.push_back(clr.b);
	}

public:

	virtual void Draw()
	{
		int nRectLength = static_cast<int>(m_vecRectData.size());
		if (nRectLength > 0)
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			glVertexPointer(3, GL_FLOAT, 6 * sizeof(float), &m_vecRectData[0]);
			glColorPointer(3, GL_FLOAT, 6 * sizeof(float), &m_vecRectData[3]);
			glDrawArrays(GL_QUADS, 0, (GLsizei)(nRectLength / 6));
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);
		}

		for (auto& itVecLineData : m_mapLineData)
		{
			auto& vecLineData = itVecLineData.second;

			if (vecLineData.empty())
				continue;

			glLineWidth(itVecLineData.first);

			size_t szLineLength = static_cast<int>(vecLineData.size());

			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			glVertexPointer(3, GL_FLOAT, 6 * sizeof(float), &vecLineData[0]);
			glColorPointer(3, GL_FLOAT, 6 * sizeof(float), &vecLineData[3]);
			glDrawArrays(GL_LINES, 0, (GLsizei)(szLineLength / 6));
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);
		}
	}

	virtual void Clear()
	{
		m_mapLineData.clear();
		m_vecRectData.clear();
	}

protected:
	std::map<float, std::vector<float>> m_mapLineData;
	std::vector<float>					m_vecRectData;
};

#endif // !GL_SHAPE_RENDER_H