
#ifndef CONSOLE_TYPE_H
#define CONSOLE_TYPE_H


#include "console_def.h"

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

template<typename T>
struct tagColor
{
	T r;
	T g;
	T b;
};

template<typename T>
struct tagPoint
{
	T x;
	T y;
};

typedef tagColor<float> ConsoleColor;
typedef tagColor<float> ConsoleGpColor;
typedef tagPoint<float> ConsoleGpPoint;

typedef struct tagConsoleCellIndex
{
	int m_iX;
	int m_iY;
} ConsoleCellIndex;


#endif // CONSOLE_TYPE_H

