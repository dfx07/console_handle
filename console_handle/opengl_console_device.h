#pragma once
#include "console_device.h"
#include <Windows.h>
#include "GL/glew.h"
#include "GL/wglew.h"
#include <assert.h>
#include "console_handle.h"

typedef struct WindowRender
{
	HWND m_hWnd = NULL;
	HDC m_hDC = NULL;
	HGLRC m_hGLRC = NULL;

} *WindowRenderPtr;

/******************************************************************************/
/*OpenGLConsoleDeviceContext*/

class OpenGLDeviceContext : public DeviceContext, public std::exception
{
#define ADD_ATRIBUTE(attribs, name, value)\
{\
    assert((size_t) attribCount < attribsize);\
    attribs[attribCount++] = name;  \
    attribs[attribCount++] = value; \
}

#define END_ATRIBUTE(attribs)\
{\
    assert((size_t) attribCount < attribsize); \
    attribs[attribCount++] = 0;  \
}

#pragma comment (lib,"opengl32.lib")
#pragma comment (lib,"glew32.lib")

public:
	OpenGLDeviceContext(DeviceContextConfig config)
		: m_Config(config)
	{
		if (!init_opengl_extensions())
		{
			throw std::exception("init opengl extensions failed !");
		}
	}

	~OpenGLDeviceContext()
	{
		DeleteContext();
	}

protected:

	// Register window class
	static bool register_window_class(const TCHAR* strClassName, WNDPROC Proc, HINSTANCE hInst)
	{
		WNDCLASSEX  wClass;
		ZeroMemory(&wClass, sizeof(WNDCLASSEX));
		wClass.cbClsExtra = NULL;
		wClass.cbSize = sizeof(WNDCLASSEX);
		wClass.cbWndExtra = NULL;
		wClass.hbrBackground = (HBRUSH)(COLOR_MENU);
		wClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wClass.hIconSm = NULL;
		wClass.hInstance = hInst;
		wClass.lpfnWndProc = (WNDPROC)Proc;
		wClass.lpszClassName = strClassName;
		wClass.lpszMenuName = NULL;
		wClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

		return !!RegisterClassEx(&wClass);
	}

	// Load library OpenGL extension use 3rd library
	static bool load_opengl_extensions()
	{
		return glewInit() == GLEW_OK;
	}

	// Create dummp OpenGL context and load OpenGL extension
	static bool init_opengl_extensions()
	{
		static bool s_binit_opengl_extensions_done = false;

		if (s_binit_opengl_extensions_done)
			return true;

		// Before we can load extensions, we need a dummy OpenGL context, created using a dummy window.
		// We use a dummy window because you can only set the pixel format for a window once. For the
		// real window, we want to use wglChoosePixelFormatARB (so we can potentially specify options
		// that aren't available in PIXELFORMATDESCRIPTOR), but we can't load and use that before we
		// have a context.
		if (!register_window_class(_T("DummyClass"), DefWindowProc, GetModuleHandle(NULL)))
		{
			return false;
		}

		HWND hWndDummy = CreateWindowEx(
			0, _T("DummyClass"), _T("Dummy OpenGL Window"), 0,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			0, 0, NULL, 0);

		PIXELFORMATDESCRIPTOR pixelFormat = {
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
			32,                   // Colordepth of the framebuffer.
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			24,                   // Number of bits for the depthbuffer
			8,                    // Number of bits for the stencilbuffer
			0,                    // Number of Aux buffers in the framebuffer.
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};

		HDC hDCDummy = ::GetDC(hWndDummy);
		int iPixelFormat = ChoosePixelFormat(hDCDummy, &pixelFormat);
		SetPixelFormat(hDCDummy, iPixelFormat, &pixelFormat);

		HGLRC hHGLRCDummy = wglCreateContext(hDCDummy);

		s_binit_opengl_extensions_done = wglMakeCurrent(hDCDummy, hHGLRCDummy) && load_opengl_extensions();

		// Delete dummy OpenGL context
		::wglMakeCurrent(hDCDummy, 0);
		::wglDeleteContext(hHGLRCDummy);
		::ReleaseDC(hWndDummy, hDCDummy);
		::DestroyWindow(hWndDummy);

		return s_binit_opengl_extensions_done;
	}

public:

	virtual bool CreateContext(void* handle)
	{
		HWND hWnd = static_cast<HWND>(handle);

		DeleteContext();

		m_pRender.m_hWnd = hWnd;

		int iPixelFormat; unsigned int num_formats = 0;
		m_pRender.m_hDC = ::GetDC(m_pRender.m_hWnd);

		if (m_pRender.m_hWnd == NULL || m_pRender.m_hDC == NULL)
			return false;

		// Get pixel format attributes through "modern" extension
		if (m_Config.ValidFlag(DEVICE_CONTEXT_USE_OPENGLEX))
		{
			const int attribsize = 47;
			int pixelAttribs[attribsize];
			int attribCount = 0;

			ADD_ATRIBUTE(pixelAttribs, WGL_DRAW_TO_WINDOW_ARB, GL_TRUE);
			ADD_ATRIBUTE(pixelAttribs, WGL_SUPPORT_OPENGL_ARB, GL_TRUE);
			ADD_ATRIBUTE(pixelAttribs, WGL_DOUBLE_BUFFER_ARB, GL_TRUE);
			ADD_ATRIBUTE(pixelAttribs, WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB);
			ADD_ATRIBUTE(pixelAttribs, WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB);
			ADD_ATRIBUTE(pixelAttribs, WGL_COLOR_BITS_ARB, 32);
			ADD_ATRIBUTE(pixelAttribs, WGL_DEPTH_BITS_ARB, 24);
			ADD_ATRIBUTE(pixelAttribs, WGL_STENCIL_BITS_ARB, 8);

			if (m_Config.ValidFlag(DEVICE_CONTEXT_ANTIALIAS))
			{
				// Enable multisampling + Number of samples
				ADD_ATRIBUTE(pixelAttribs, WGL_SAMPLE_BUFFERS_ARB, GL_TRUE);
				ADD_ATRIBUTE(pixelAttribs, WGL_SAMPLES_ARB, m_Config.GetAntiliasingLevel());
			}

			END_ATRIBUTE(pixelAttribs);

			wglChoosePixelFormatARB(m_pRender.m_hDC, pixelAttribs, 0, 1, &iPixelFormat, &num_formats);

			PIXELFORMATDESCRIPTOR pfd;
			DescribePixelFormat(m_pRender.m_hDC, iPixelFormat, sizeof(pfd), &pfd);
			SetPixelFormat(m_pRender.m_hDC, iPixelFormat, &pfd);

			// Specify that we want to create an OpenGL x.x core profile context
			int gl_attribs[] = {
				WGL_CONTEXT_MAJOR_VERSION_ARB, 1,
				WGL_CONTEXT_MINOR_VERSION_ARB, 5,
				WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
				0,
			};

			m_pRender.m_hGLRC = wglCreateContextAttribsARB(m_pRender.m_hDC, 0, gl_attribs);
		}
		// Get pixel format attributes through legacy PFDs
		else
		{
			PIXELFORMATDESCRIPTOR pfd = {
				sizeof(PIXELFORMATDESCRIPTOR),
				1,
				PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
				PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
				32,                   // Colordepth of the framebuffer.
				0, 0, 0, 0, 0, 0,
				0,
				0,
				0,
				0, 0, 0, 0,
				24,                   // Number of bits for the depthbuffer
				8,                    // Number of bits for the stencilbuffer
				0,                    // Number of Aux buffers in the framebuffer.
				PFD_MAIN_PLANE,
				0,
				0, 0, 0
			};
			iPixelFormat = ChoosePixelFormat(m_pRender.m_hDC, &pfd);
			SetPixelFormat(m_pRender.m_hDC, iPixelFormat, &pfd);

			m_pRender.m_hGLRC = wglCreateContext(m_pRender.m_hDC);
		}

		if (!MakeCurrentContext())
		{
			DeleteContext();
			return false;
		}

		return true;
	}

	virtual bool IsValid() const noexcept
	{
		return !!(m_pRender.m_hGLRC);
	}

	virtual void SetConfig(DeviceContextConfig config)
	{
		m_Config = config;
	}

	virtual void* Render() noexcept
	{
		return &m_pRender.m_hDC;
	}

	virtual void DeleteContext()
	{
		// Release device context
		if (m_pRender.m_hWnd && m_pRender.m_hDC)
			ReleaseDC(m_pRender.m_hWnd, m_pRender.m_hDC);

		// Delete the rendering context
		wglDeleteContext(m_pRender.m_hGLRC);

		// reset
		m_pRender.m_hDC = NULL;
		m_pRender.m_hGLRC = NULL;
	}

	virtual bool MakeCurrentContext()
	{
		if (IsValid())
		{
			return !!wglMakeCurrent(m_pRender.m_hDC, m_pRender.m_hGLRC);
		}

		return false;
	}

	virtual void SwapBuffer()
	{
		::SwapBuffers(m_pRender.m_hDC);
	}

protected:
	WindowRender		m_pRender;
	DeviceContextConfig	m_Config;
};

interface IConsoleObjectRender
{
protected:
	DeviceContext* m_pContext{ nullptr };

public:
	virtual void SetContext(DeviceContext* pContext) { m_pContext = pContext; }
	virtual void SetView(ConsoleView* pView) = 0;

	virtual void Draw() = 0;
	virtual void Clear() = 0;
};

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
};