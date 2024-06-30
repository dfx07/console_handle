#ifndef CONSOLE_HANDLE_H
#define CONSOLE_HANDLE_H

#include "console_device.h"
#include "console_model.h"
#include "console_view.h"

#include "console_def.h"

#include <map>

#pragma warning( disable : 26812)

class ConsoleHandle;
class ConsoleGraphics;

// Handle Event
interface ConsoleHandleEvent
{
public:
	virtual void OnMouseEvent() = 0;
	virtual void OnKeyBoardEvent() = 0;
	virtual void OnResizeEvent() = 0;

protected:
	typedef void(*typeFunOnMouseEvent) (ConsoleHandle* handle, MouseEventInfo* mouseInfo);
	typedef void(*typeFunOnKeyboardEvent)(ConsoleHandle* handle, KeyBoardEventInfo* keyboardInfo);
	typedef void(*typeFunOnResizeEvent) (ConsoleHandle* handle);
	typedef void(*typeFunOnDraw) (ConsoleHandle* handle, ConsoleGraphics* pView);

public:
	void SetMouseEventCallback(typeFunOnMouseEvent funCallBack) noexcept { m_funOnMouseEvent = funCallBack; }
	void SetKeyboardEventCallback(typeFunOnKeyboardEvent funCallBack) noexcept { m_funOnKeyboardEvent = funCallBack; }
	void SetResizeEventCallback(typeFunOnResizeEvent funCallBack) noexcept { m_funOnResizeEvent = funCallBack; }
	void SetDrawCallback(typeFunOnDraw funCallBack) noexcept { m_funOnDraw = funCallBack; }

protected:
	typeFunOnMouseEvent		m_funOnMouseEvent{ nullptr };
	typeFunOnKeyboardEvent	m_funOnKeyboardEvent{ nullptr };
	typeFunOnResizeEvent	m_funOnResizeEvent{ nullptr };
	typeFunOnDraw			m_funOnDraw{ nullptr };

	MouseEventInfo			m_MouseEvent;
	KeyBoardEventInfo		m_KeyboardEvent;

public:
	virtual void PollEvent() = 0;
	virtual void WaitEvent() = 0;
	virtual MouseEventInfo* GetMouseEvent() noexcept { return &m_MouseEvent; }
	virtual KeyBoardEventInfo* GetKeyboardEvent() noexcept { return &m_KeyboardEvent; }
};

interface ConsolePlatform
{
	virtual void* GetHandle() noexcept = 0;
};

// Console font
interface ConsoleFont : public ConsolePlatform
{
	virtual ~ConsoleFont() = 0;
	virtual bool LoadFont(const char* font_name, unsigned int font_size, ConsoleFontType font_type) = 0;
	virtual void UnLoad() noexcept= 0;
	virtual ConsoleFont* Clone() = 0;
	virtual void ChangeSize(unsigned int font_size) noexcept = 0;

	virtual std::string GetFontName() noexcept = 0;
	virtual unsigned int GetFontSize() noexcept = 0;
};

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

	virtual void RemoveAll()
	{
		m_Fonts.clear();
	}

	virtual void Remove(unsigned int font_size)
	{
		m_Fonts.erase(font_size);
	}

protected:
	std::map<unsigned int, ConsoleFontPtr> m_Fonts;
};

class ConsoleFontManager
{
	using ConsoleFontPtr = std::shared_ptr<ConsoleFont>;


public:
	struct ConsoleSearchFont
	{
		std::string name;
		unsigned int size;
	};


public:
	void Add(ConsoleFontPtr font)
	{
		//m_Mana.insert({ ConsoleFontKey{font->GetFontName(), font->GetFontSize()}, font });
	}

	void Remove(const char* font_name, unsigned int font_size = 0)
	{
		//m_Mana.erase(ConsoleFontKey{ font_name, font_size });
	}

	ConsoleFontPtr Get(const char* font_name, unsigned int font_size = 0)
	{
		return nullptr;
	}

protected:
	std::map<std::string, ConsoleFontSizeInfo> m_Mana;
};

// Console Handle
class ConsoleHandle : public ConsolePlatform
{
	using ConsoleDevicePtr = std::shared_ptr<ConsoleDevice>;
	using ConsoleBoardModelDataPtr = std::shared_ptr<ConsoleBoardModelData>;

public:
	virtual void* GetHandleParent() noexcept = 0 ;
	virtual bool Create(const TCHAR* strTitle, int xpos, int ypos, unsigned int nWidth, const int nHeight) = 0;
	virtual void SetWindowSize(const int nRow, const int nCol) = 0;
	virtual void SetWindowPosition(const int xPos, const int yPos) = 0;
	virtual void SetWindowCenter() = 0;
	virtual void SetCellSize(const int nWidth, const int nHeight) = 0;
	virtual void ShowCursor(bool bShow) = 0;
	virtual void Show() = 0;
	virtual void Hide() = 0;
	virtual void SetFont(const TCHAR* strFont) = 0;
	virtual void SetTitle(const TCHAR* strTitle) = 0;
	virtual bool Closed() const = 0;
	virtual void Draw() = 0;

public:
	ConsoleHandle() { };
	virtual ~ConsoleHandle() { };

protected:
	virtual bool CreateBoardView(const unsigned int nWidth, const unsigned int nHeight) = 0;
	virtual bool CreateBoardModel(const int nRow, const int nCol) = 0;

protected:
	bool					m_bClosed{ false };
	unsigned int			m_uWidth{ 0 };
	unsigned int			m_uHeight{ 0 };

	unsigned int			m_nRows{ 0 };
	unsigned int			m_nCols{ 0 };

	bool					m_bVisible{ true };
	bool					m_bEnable{ true };
	bool					m_bFullScreen{ true };
	TCHAR					m_strTitle[80]{ 0 };

	ConsoleBoardModelDataPtr m_pModelData;
	ConsoleBoardView		 m_View;
	ConsoleDevicePtr		 m_pDevice{ nullptr };
};

#endif // !CONSOLDE_HANDLE_H