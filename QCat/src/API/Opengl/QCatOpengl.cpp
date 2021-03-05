#include "qcpch.h"
#include "QCatOpengl.h"
#include "QCat/Core/Log.h"
//#include <gl/GL.h>


namespace QCat
{
	HGLRC QCatOpengl::rc;

	typedef void* (APIENTRYP PFNWGLGETPROCADDRESSPROC_PRIVATE)(const char*);
	static PFNWGLGETPROCADDRESSPROC_PRIVATE gladGetProcAddressPtr;
	static HMODULE module;
	void Set_Proc()
	{
	    module = GetModuleHandle("opengl32.dll");
		void (*tmp)(void);
		tmp = (void(*)(void)) GetProcAddress(module, "wglGetProcAddress");
		gladGetProcAddressPtr = (PFNWGLGETPROCADDRESSPROC_PRIVATE)tmp;
	}
	void* Get_Proc(const char* name)
	{
		// with wglGetProcAddress you can get function pointer about functions whics is above 1.2 version
		// if  you want to get function pointer bellow 1.2 version you should use GetProcAddress
		// if you are trying to get function pointer through wrong method you will get NULL pointer
		void* result =nullptr;
		if(gladGetProcAddressPtr!=nullptr)
			result = gladGetProcAddressPtr(name);		
		if (result == NULL)
			result = (void*)GetProcAddress(module, name);
		return result;
	}
	QCatOpengl::~QCatOpengl()
	{
		if (dc)
			wglMakeCurrent(dc, NULL);
		if (rc)
			wglDeleteContext(rc);
	}
	void QCatOpengl::Initialize(HWND hwnd)
	{
		QCAT_PROFILE_FUNCTION();
		TCHAR className[255];
		GetClassName(hwnd, className, _countof(className));

		HWND fakeWND = CreateWindow(
			className, className,		 // window class, title
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN,	 // style
			0, 0,								 // position x, y
			1, 1,								 // width, height
			NULL, NULL,							 // parent window, menu
			GetModuleHandle(NULL), NULL);							 // instance, param

		HDC fakeDC = GetDC(fakeWND);			 // Device Context

		// from wingdi.h Description of pixel format
		PIXELFORMATDESCRIPTOR pfd;
		ZeroMemory(&pfd, sizeof(pfd));
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cAccumBits = 8;
		pfd.cDepthBits = 24;
		pfd.cStencilBits = 8;

		// match appropritate pixel foramt for device to given pixel format description
		int pfdID = ChoosePixelFormat(fakeDC, &pfd);
		if (pfdID == 0)
			QCAT_CORE_ERROR("Choose Pixel Format Failed : OpegnGl Init");
		
		// set the pixel format of device context to format specified by description
		if(SetPixelFormat(fakeDC,pfdID,&pfd)==false)
			QCAT_CORE_ERROR("Set Pixel Format Failed : OpegnGl Init");

		// create OpenGl rendering context that is suitable for specified device
		// this context has same pixel foramt as the device context
		HGLRC fakeRc = wglCreateContext(fakeDC);

		// make rendering context calling thread's current rendering context
		// it means every oepngl calls will be drawn onto specified hdc
		if (wglMakeCurrent(fakeDC, fakeRc) == false)
			QCAT_CORE_ERROR("Make Current Failed : OpenGl Init");

		
		// at this point you can get pointers to modern opengl function now
		
		// retrieve the opengl function
		PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
		wglChoosePixelFormatARB = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(wglGetProcAddress("wglChoosePixelFormatARB"));
		QCAT_CORE_ASSERT(wglChoosePixelFormatARB != nullptr, "Error! Cant retrieve the opengl Function : wglChoosePixelFormatARB");

		PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
		wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
		QCAT_CORE_ASSERT(wglCreateContextAttribsARB != nullptr, "Error! Cant retrieve the opengl Function : wglCreateContextAttribsARB");

		const int pixelAttribs[] = {
				WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
				WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
				WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
				WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
				WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
				WGL_COLOR_BITS_ARB, 24,
				WGL_ALPHA_BITS_ARB, 8,
				WGL_DEPTH_BITS_ARB, 24,
				WGL_STENCIL_BITS_ARB, 8,
				WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
				WGL_SAMPLES_ARB, 0,
				0
		};

		int pixelFormatID; UINT numFormats;
		bool status = wglChoosePixelFormatARB(dc, pixelAttribs, NULL, 1, &pixelFormatID, &numFormats);
		QCAT_CORE_ASSERT(status != false || numFormats != 0, "wglChoosePixelFormatARB failed");

		PIXELFORMATDESCRIPTOR PFD;
		DescribePixelFormat(dc, pixelFormatID, sizeof(PFD), &PFD);
		SetPixelFormat(dc, pixelFormatID, &PFD);

		const int major_min = 4, minor_min = 6;
		int  contextAttribs[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, major_min,
			WGL_CONTEXT_MINOR_VERSION_ARB, minor_min,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0
		};

		if (!rc)
		{
			rc = wglCreateContextAttribsARB(dc, 0, contextAttribs);
			QCAT_CORE_ASSERT(rc != NULL, "wglCreateContextAttribsARB failed");
			Set_Proc();
			int statuss = gladLoadGLLoader(&Get_Proc);
			wgl.wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)Get_Proc("wglSwapIntervalEXT");
			wgl.wglSwapIntervalEXT(1);
		}
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(fakeRc);
		ReleaseDC(fakeWND, fakeDC);
		DestroyWindow(fakeWND);
		bool suceed = wglMakeCurrent(dc, rc);
		QCAT_CORE_ASSERT(suceed != false, "wglMakeCurrent() failed");

		std::string version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
		QCAT_CORE_TRACE("OPENGL VERSION {0}", version);
	}

	void QCatOpengl::MakeCurrent()
	{
		bool suceed = wglMakeCurrent(dc, rc);

	}

	void QCatOpengl::Init(void* pHandle)
	{
		HWND hwnd = static_cast<HWND>(pHandle);
		QCAT_CORE_ASSERT(hwnd != NULL, "window handle is NULL!");

		dc = GetDC(hwnd);
		Initialize(hwnd);
	}
	void QCatOpengl::Begin()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}
	void QCatOpengl::End()
	{
		QCAT_PROFILE_FUNCTION();
		SwapBuffers(dc);
	}
}