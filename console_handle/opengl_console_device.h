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
		wClass.cbClsExtra		= NULL;
		wClass.cbSize			= sizeof(WNDCLASSEX);
		wClass.cbWndExtra		= NULL;
		wClass.hbrBackground	= (HBRUSH)(COLOR_MENU);
		wClass.hCursor			= LoadCursor(NULL, IDC_ARROW);
		wClass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
		wClass.hIconSm			= NULL;
		wClass.hInstance		= hInst;
		wClass.lpfnWndProc		= (WNDPROC)Proc;
		wClass.lpszClassName	= strClassName;
		wClass.lpszMenuName		= NULL;
		wClass.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

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

	virtual bool IsValid()
	{
		return (m_pRender.m_hGLRC);
	}

	virtual void SetConfig(DeviceContextConfig config)
	{
		m_Config = config;
	}

	virtual void* Render()
	{
		return &m_pRender.m_hDC;
	}

	virtual void DeleteContext()
	{
		// Release device context
		if(m_pRender.m_hWnd && m_pRender.m_hDC)
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


/******************************************************************************/
/*OpenGLConsoleDevice*/

class OpenGLConsoleDevice : public ConsoleDevice, public ConsoleDeviceIO
{
	enum { MAX_RENDER_DATA = 10000 };

public:
	OpenGLConsoleDevice(DeviceContextConfig config)
	{
		m_vecLineDataRenders.reserve(MAX_RENDER_DATA);
		m_vecRectDataRenders.reserve(MAX_RENDER_DATA);

		m_pContext = new OpenGLDeviceContext(config);
	}

	~OpenGLConsoleDevice()
	{
		m_pContext->DeleteContext();

		delete m_pContext;
	}

public:
	bool CreateDeviceContext(ConsoleHandle* pHandle)
	{
		HWND hWnd = static_cast<HWND>(pHandle->GetHandle());

		bool bCreateDone = m_pContext->CreateContext(hWnd);

		return bCreateDone;
	}

public:
	virtual void SetGraphics(ConsoleGraphics* pGraphic)
	{
		m_pGraphics = pGraphic;
	}

	virtual bool Begin(ConsoleGraphics* pGraphic)
	{
		if (!pGraphic)
			return false;

		SetGraphics(pGraphic);

		return m_pContext->MakeCurrentContext();
	}

	virtual void End()
	{
		m_pContext->SwapBuffer();
	}

	virtual void Draw()
	{
		m_pContext->MakeCurrentContext();
	}

	virtual void Update()
	{
		CreateRenderDataLines();
		CreateRenderDataRectangles();
	}

	virtual void Clear()
	{
		m_vecLineDataRenders.clear();
		m_vecRectDataRenders.clear();
	}

protected:
	virtual void CreateRenderDataLines()
	{
		if (!m_pGraphics)
			return;

		auto drawBufferLines = m_pGraphics->GetBufferData()->GetDrawBufferLines();
		size_t nBufferSize = drawBufferLines.size();

		for (int i = 0; i < nBufferSize; i++)
		{
			m_vecLineDataRenders.push_back(drawBufferLines[i].second.pt1.x);
			m_vecLineDataRenders.push_back(drawBufferLines[i].second.pt1.y);
			m_vecLineDataRenders.push_back(static_cast<float>(m_nZStart + drawBufferLines[i].first));

			m_vecLineDataRenders.push_back(drawBufferLines[i].second.col.r);
			m_vecLineDataRenders.push_back(drawBufferLines[i].second.col.g);
			m_vecLineDataRenders.push_back(drawBufferLines[i].second.col.b);

			m_vecLineDataRenders.push_back(drawBufferLines[i].second.pt2.x);
			m_vecLineDataRenders.push_back(drawBufferLines[i].second.pt2.y);
			m_vecLineDataRenders.push_back(static_cast<float>(m_nZStart + drawBufferLines[i].first));

			m_vecLineDataRenders.push_back(drawBufferLines[i].second.col.r);
			m_vecLineDataRenders.push_back(drawBufferLines[i].second.col.g);
			m_vecLineDataRenders.push_back(drawBufferLines[i].second.col.b);
		}
	}

	virtual void CreateRenderDataRectangles()
	{
		if (!m_pGraphics)
			return;

		auto drawBufferRects = m_pGraphics->GetBufferData()->GetDrawBufferRects();
		size_t nBufferSize = drawBufferRects.size();

		for (int i = 0; i < nBufferSize; i++)
		{
			m_vecLineDataRenders.push_back(drawBufferRects[i].second.pt.x);
			m_vecLineDataRenders.push_back(drawBufferRects[i].second.pt.y);
			m_vecLineDataRenders.push_back(static_cast<float>(m_nZStart + drawBufferRects[i].first));

			m_vecLineDataRenders.push_back(drawBufferRects[i].second.col.r);
			m_vecLineDataRenders.push_back(drawBufferRects[i].second.col.g);
			m_vecLineDataRenders.push_back(drawBufferRects[i].second.col.b);

			m_vecLineDataRenders.push_back(drawBufferRects[i].second.pt.x);
			m_vecLineDataRenders.push_back(drawBufferRects[i].second.pt.y + drawBufferRects[i].second.height);
			m_vecLineDataRenders.push_back(static_cast<float>(m_nZStart + drawBufferRects[i].first));

			m_vecLineDataRenders.push_back(drawBufferRects[i].second.col.r);
			m_vecLineDataRenders.push_back(drawBufferRects[i].second.col.g);
			m_vecLineDataRenders.push_back(drawBufferRects[i].second.col.b);

			m_vecLineDataRenders.push_back(drawBufferRects[i].second.pt.x + drawBufferRects[i].second.width);
			m_vecLineDataRenders.push_back(drawBufferRects[i].second.pt.y + drawBufferRects[i].second.height);
			m_vecLineDataRenders.push_back(static_cast<float>(m_nZStart + drawBufferRects[i].first));

			m_vecLineDataRenders.push_back(drawBufferRects[i].second.col.r);
			m_vecLineDataRenders.push_back(drawBufferRects[i].second.col.g);
			m_vecLineDataRenders.push_back(drawBufferRects[i].second.col.b);

			m_vecLineDataRenders.push_back(drawBufferRects[i].second.pt.x + drawBufferRects[i].second.height);
			m_vecLineDataRenders.push_back(drawBufferRects[i].second.pt.y);
			m_vecLineDataRenders.push_back(static_cast<float>(m_nZStart + drawBufferRects[i].first));

			m_vecLineDataRenders.push_back(drawBufferRects[i].second.col.r);
			m_vecLineDataRenders.push_back(drawBufferRects[i].second.col.g);
			m_vecLineDataRenders.push_back(drawBufferRects[i].second.col.b);
		}
	}

protected:
	int m_nZStart = 100;

	std::vector<float> m_vecLineDataRenders;
	std::vector<float> m_vecRectDataRenders;

	ConsoleGraphics* m_pGraphics{ nullptr };
	DeviceContext* m_pContext{ nullptr };
};