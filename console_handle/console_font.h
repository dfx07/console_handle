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

///////////////////////////////////////////////////////////////////////////////
/*ConsoleFont class*/

interface ConsoleFont : public ConsolePlatform
{
	virtual ~ConsoleFont() = 0;
	virtual bool LoadFont(const wchar_t* font_name, unsigned int font_size, ConsoleFontType font_type) = 0;
	virtual void UnLoad() noexcept = 0;
	virtual ConsoleFont* Clone() = 0;
	virtual void ChangeSize(unsigned int font_size) noexcept = 0;

	virtual std::wstring GetFontName() noexcept = 0;
	virtual unsigned int GetFontSize() noexcept = 0;
};


///////////////////////////////////////////////////////////////////////////////
/*ConsoleFontSizeInfo class*/

class ConsoleFontSizeInfo
{
	using ConsoleFontPtr = std::shared_ptr<ConsoleFont>;

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


///////////////////////////////////////////////////////////////////////////////
/*ConsoleFontManager class*/

class ConsoleFontManager
{
	using ConsoleFontPtr = std::shared_ptr<ConsoleFont>;

public:
	struct ConsoleSearchFont
	{
		std::wstring name;
		unsigned int size;
	};

public:
	bool Add(ConsoleFontPtr font)
	{
		if (!font || font.get())
			return;

		bool bAddOk = false;

		auto itFontFound = m_Fontmana.find(font->GetFontName());

		if (itFontFound == m_Fontmana.end())
		{
			ConsoleFontSizeInfo fontInfo;
			bAddOk = fontInfo.Add(font);
			m_Fontmana.insert({ font->GetFontName(), fontInfo });
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
		auto itFontList = m_Fontmana.find(font_name);

		if (itFontList == m_Fontmana.end())
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

	ConsoleFontPtr Get(const wchar_t* font_name, unsigned int font_size = 0) const
	{
		auto itFontList = m_Fontmana.find(font_name);

		if (itFontList == m_Fontmana.end() || itFontList->second.GetSize() <= 0)
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

protected:
	std::map<std::wstring, ConsoleFontSizeInfo> m_Fontmana;
};