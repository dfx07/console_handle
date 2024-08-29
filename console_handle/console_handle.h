#ifndef CONSOLE_HANDLE_H
#define CONSOLE_HANDLE_H

#include "console_device.h"
#include "console_model.h"
#include "console_view.h"

#include "console_if.h"

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

// Console Handle
class ConsoleHandle : public ConsolePlatform
{
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
	virtual void Update() = 0;
	virtual ConsoleViewPtr GetView() { return m_pView; }
	unsigned int GetRows() const noexcept { return m_nRows; }
	unsigned int GetColumns() const noexcept { return m_nCols; }

public:
	ConsoleHandle() { };
	virtual ~ConsoleHandle() { };

protected:
	virtual bool CreateBoardView(const unsigned int nWidth, const unsigned int nHeight) = 0;
	virtual bool CreateBoardModel(const int nRow, const int nCol) = 0;

protected:
	bool		 m_bClosed{ false };
	unsigned int m_uWidth{ 0 };
	unsigned int m_uHeight{ 0 };

	unsigned int m_nRows{ 0 };
	unsigned int m_nCols{ 0 };

	bool		 m_bVisible{ true };
	bool		 m_bEnable{ true };
	bool		 m_bFullScreen{ true };
	TCHAR		 m_strTitle[80]{ 0 };

	ConsoleBoardModelDataPtr m_pModelData{ nullptr };
	ConsoleViewPtr			 m_pView{ nullptr };
	ConsoleDevicePtr		 m_pDevice{ nullptr };
};

#endif // !CONSOLDE_HANDLE_H