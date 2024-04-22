#pragma once

#include <vector>

#undef interface

#ifndef interface
#define interface struct
#endif

#undef TCHAR

#ifdef _UNICODE
#define _T(txt) L##txt
typedef wchar_t TCHAR;
#else
#define _T(txt)
typedef char TCHAR;
#endif

struct ConsoleGpColor
{
	float r;
	float g;
	float b;
};

struct ConsoleGpPoint
{
	float x;
	float y;
};

struct ConsoleColor
{
	int r;
	int g;
	int b;
};

struct ConsoleCellIndex
{
	int m_iX;
	int m_iY;
};

