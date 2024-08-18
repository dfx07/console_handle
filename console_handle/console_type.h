
#ifndef CONSOLE_TYPE_H
#define CONSOLE_TYPE_H

#include <memory>
#include "console_def.h"
#include "xgeotype.h"

// xy is position : x (left->right), y (top->bottom)
typedef struct tagConsoleMousePos
{
	int x{ 0 };
	int y{ 0 };

} ConsoleMousePos, * PConsoleMousePos;

typedef struct tagMouseEventInfo
{
	ConsoleMouseButton	m_MouseButton{ MOUSE_BUTTON_NONE };
	ConsoleMousePos		m_MousePos;
	int					m_MouseState{ MOUSE_NONE_STATE };

} MouseEventInfo, * PMouseEventInfo;

typedef struct tagKeyBoardEventInfo
{
	ConsoleKeyboard m_eKey;
	ConsoleKeyboardState m_eState{ KEYBOARD_NONE_STATE };

} KeyBoardEventInfo, * PKeyBoardEventInfo;

typedef struct tagConsoleHandleState
{
	int			 xpos;
	int			 ypos;
	unsigned int width;
	unsigned int height;

} ConsoleHandleState, * PConsoleHandleState;

typedef _t3Tag<float> ConsolePoint;
typedef _t3Tag<float> ConsoleColor;
typedef std::wstring  ConsoleString;

typedef struct tagConsoleCellIndex
{
	int m_iX;
	int m_iY;
} ConsoleCellIndex;

class ConsoleGraphics;
typedef std::shared_ptr<ConsoleGraphics> ConsoleGraphicsPtr;

class ConsoleDevice;
typedef std::shared_ptr<ConsoleDevice> ConsoleDevicePtr;

class ConsoleDeviceControl;
typedef std::shared_ptr<ConsoleDeviceControl> ConsoleDeviceControlPtr;

class ConsoleBoardModelData;
typedef std::shared_ptr<ConsoleBoardModelData> ConsoleBoardModelDataPtr;

class ConsoleFont;
typedef std::shared_ptr<ConsoleFont> ConsoleFontPtr;

class ConsoleFontManager;
typedef std::shared_ptr<ConsoleFontManager> ConsoleFontManagerPtr;

class ConsoleView;
typedef std::shared_ptr<ConsoleView> ConsoleViewPtr;

#endif // CONSOLE_TYPE_H

