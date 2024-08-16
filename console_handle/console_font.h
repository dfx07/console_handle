////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************
* Copyright (C) 2023-2024 thuong.nv <thuong.nv.mta@gmail.com>
* MIT software Licencs, see the accompanying
* http://www.opensource.org/licenses/mit-license.php
*
/***********************************************************************************
* @brief : Console font
* @file  : console_font.h
* @create: Aug 16, 2024
* @note  : For conditions of distribution and use, see copyright notice in readme.txt
***********************************************************************************/

#pragma once

#include "console_if.h"
#include <string>
#include <memory>
#include <map>

typedef enum tagConsoleFontType
{
	Normal,
	Bold,
	Thin,
} ConsoleFontType;

/////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************/
// ConsoleFontKey struct

typedef struct _ConsoleFontKey
{
public:
	bool operator<(const _ConsoleFontKey& otkey) const noexcept
	{
		if (size == otkey.size)
			return name < otkey.name;
		return size < otkey.size;
	}

	bool IsEmpty() const noexcept
	{
		return (name.empty() || size <= 0);
	}

public:
	ConsoleString name{ _T("") };
	unsigned int  size{ 0 };
} ConsoleFontKey;

/////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************/
// ConsoleFont class

class ConsoleFont : public ConsolePlatform
{
public:
	virtual ~ConsoleFont() { };
	virtual bool Load(const ConsoleString font_name, unsigned int font_size, ConsoleFontType font_type) = 0;
	virtual void UnLoad() noexcept = 0;
	virtual ConsoleFont* Clone() = 0;
	virtual void ChangeSize(unsigned int font_size) noexcept = 0;
	virtual ConsoleFontPtr CreateConsoleFontIndirect(const ConsoleString font_name, unsigned int font_size, ConsoleFontType font_type) noexcept = 0;
	virtual ConsoleFontPtr CreateConsoleFontIndirect(const ConsoleFontKey& fontKey, ConsoleFontType font_type = ConsoleFontType::Normal) noexcept = 0;

	virtual ConsoleString GetFontName() noexcept = 0;
	virtual unsigned int GetFontSize() noexcept = 0;
};

/////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************/
// ConsoleFontSizeInfo class

class ConsoleFontSizeInfo
{
protected:
	ConsoleFontPtr AddFontWithSize(ConsoleFontPtr pFont, unsigned int font_size)
	{
		ConsoleFontPtr pNewFont;
		pNewFont.reset(pFont->Clone());

		if (!pNewFont || !pNewFont.get())
		{
			assert(0);
			return nullptr;
		}

		m_Fonts.insert(std::make_pair(font_size, pNewFont));

		return pNewFont;
	}

public:

	virtual ConsoleFontPtr Get(unsigned int font_size)
	{
		auto itFont = m_Fonts.find(font_size);
		if (itFont != m_Fonts.end())
		{
			return itFont->second;
		}

		if (m_Fonts.empty())
			return nullptr;

		auto pFont = AddFontWithSize(m_Fonts.begin()->second, font_size);

		return pFont;
	}

	virtual ConsoleFontPtr GetFirst() const
	{
		if (GetSize() <= 0)
			return nullptr;

		return m_Fonts.begin()->second;
	}

	virtual bool Add(ConsoleFontPtr font)
	{
		auto itFontFound = m_Fonts.find(font->GetFontSize());

		if (itFontFound != m_Fonts.end())
		{
			return false;
		}

		m_Fonts.insert({ font->GetFontSize(), font });
		return true;
	}

	virtual void RemoveAll() noexcept
	{
		m_Fonts.clear();
	}

	virtual void Remove(unsigned int font_size)
	{
		m_Fonts.erase(font_size);
	}

	virtual size_t GetSize() const noexcept
	{
		return m_Fonts.size();
	}

protected:
	std::map<unsigned int, ConsoleFontPtr> m_Fonts;
};

/////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************************/
// ConsoleFontManager class

class ConsoleFontManager
{
public:
	bool Add(ConsoleFontPtr font)
	{
		if (!font || !font.get())
			return false;

		bool bAddOk = false;

		auto itFontFound = m_FontMana.find(font->GetFontName());

		if (itFontFound == m_FontMana.end())
		{
			ConsoleFontSizeInfo fontInfo;
			bAddOk = fontInfo.Add(font);
			m_FontMana.insert({ font->GetFontName(), fontInfo });
		}
		else
		{
			bAddOk = itFontFound->second.Add(font);
		}

		return bAddOk;
	}

	/*
	* if font size is zero then clear all font
	*/
	void Remove(const wchar_t* font_name, unsigned int font_size = 0)
	{
		auto itFontList = m_FontMana.find(font_name);

		if (itFontList == m_FontMana.end())
			return;

		if (font_size <= 0)
		{
			itFontList->second.RemoveAll();
		}
		else
		{
			itFontList->second.Remove(font_size);
		}
	}

	void SetDefaultFont(ConsoleFontPtr pFont) noexcept
	{
		Remove(pFont->GetFontName().c_str(), pFont->GetFontSize());

		if (Add(pFont))
		{
			m_pFontDefault = pFont;
		}
		else
		{
			assert(0);
		}
	}

	ConsoleFontPtr GetDefaultFont() const noexcept
	{
		return m_pFontDefault;
	}

	ConsoleFontPtr Get(const wchar_t* font_name, unsigned int font_size = 0) const
	{
		auto itFontList = m_FontMana.find(font_name);

		if (itFontList == m_FontMana.end() || itFontList->second.GetSize() <= 0)
			return nullptr;

		if (font_size <= 0)
		{
			return itFontList->second.GetFirst();
		}
		else
		{
			return const_cast<ConsoleFontSizeInfo&>(itFontList->second).Get(font_size);
		}
	}

	ConsoleFontPtr Get(const ConsoleFontKey& key) const
	{
		return Get(key.name.c_str(), key.size);
	}

protected:
	std::map<ConsoleString, ConsoleFontSizeInfo> m_FontMana;
	ConsoleFontPtr m_pFontDefault{ nullptr };
};
