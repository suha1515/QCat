#include "qcpch.h"
#include "WindowsWindow.h"

#include "QCat/Events/ApplicationEvent.h"
#include "QCat/Events/MouseEvent.h"
#include "QCat/Events/KeyboardEvent.h"

#include "backends/imgui_impl_win32.h"
#include "QCat/Log.h"

namespace QCat
{
	Window* Window::Create(const WindowProps& props)
	{
		int width = props.Width;
		int height = props.Height;
		const char* title = props.Title.c_str();
		return new WindowsWindow(width,height,title);
	}

	WindowsWindow::WindowsWindow(int width, int height, const char* pWndName)
		:wndClassName(pWndName), wndName(pWndName), width(width), height(height), windowOpen(true)
	{
		Window::m_data.Height = height;
		Window::m_data.Width = width;
		Window::m_data.Title = pWndName;
		QCAT_CORE_INFO("Creating Window {0} ({1}, {2})", pWndName, width, height);
		//������ Ŭ���� ���
		hInstance = GetModuleHandle(nullptr);
		WNDCLASSEX wc = { 0 };
		wc.cbSize = sizeof(wc);
		wc.style = CS_OWNDC;
		wc.lpfnWndProc = MsgSetUp;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		//wc.hIcon = static_cast<HICON>(LoadImage(hInstance, IDI_APPLICATION, IMAGE_ICON, 32, 32, 0));
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = nullptr;
		wc.lpszMenuName = nullptr;
		wc.lpszClassName = wndClassName;
		//wc.hIconSm = static_cast<HICON>(LoadImage(hInstance, IDI_APPLICATION, IMAGE_ICON, 16, 16, 0));
		RegisterClassEx(&wc);

		// ������ ����� ���������Ͽ� Ŭ���̾�Ʈ ������ ����Ѵ�.
		RECT wr;
		wr.left = 0;
		wr.right = width + wr.left;
		wr.top = 0;
		wr.bottom = height + wr.top;
		//���� ����� ��Ÿ���� �����Ͽ� RECT �� ��������� ũ�⸦ ����Ѵ�.
		if ((AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE) == 0))
		{
			
		}
		//������ ����
		hWnd = CreateWindow(wndClassName, wndName,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
			nullptr, nullptr, hInstance, this
		);
		// ����üũ
		if (hWnd == nullptr)
		{
		
		}
		ShowWindow(hWnd, SW_SHOWDEFAULT);
		UpdateWindow(hWnd);

		pGfx = std::make_unique<QGfxDeviceDX11>(hWnd, width, height);
		// Raw���� �޾ƿ� ��ġ������ ����ȭ�Ѵ�
		// �Է���ġ���� �پ��� ��ġ�������� ����ڴ� �ش� ��ġ�����ؼ� �Է��� �ް����Ҷ�
		// WM_MOUSEMOVE �� WM_KEYDOWN ���� Ű���峪 ���콺 �޽������ƴ� WM_INPUT���� �ش� ��ġ������ ������ �޾ƾ��Ѵ�
		RAWINPUTDEVICE rid;
		rid.usUsagePage = 0x01; // mouse page
		rid.usUsage = 0x02; // mouse usage
		rid.dwFlags = 0;
		rid.hwndTarget = nullptr;	//raw ������ ���� �������ڵ�
		//��ġ���
		if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
		{
		
		}
	}
	WindowsWindow::~WindowsWindow()
	{
		// DLL�� ��� �� �����Ҹ� �ؾ��Ѵ�.
		UnregisterClass(wndClassName, hInstance);
		// �ڵ��� �ı��Ѵ�.
		DestroyWindow(hWnd);
	}
	QGfxDeviceDX11& WindowsWindow::Gfx()
	{
		return *pGfx;
	}
	LRESULT WindowsWindow::MsgSetUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		// �ʱ� �����찡 �����ɶ�
		if (msg == WM_NCCREATE )
		{
			// CreateWindow �Լ����� ������ ������ �Ķ���� ������ ����ϱ����� ����ȯ�� �Ѵ�.
			const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
			// lParam �ڷḦ CREATESTRUCT ����ü�� ��ȯ�� lpCreateParams ����� �����Ͽ� this �����ͷ� ����� �ڷḦ ����ȯ�Ѵ�.
			WindowsWindow* const pWnd = static_cast<WindowsWindow*>(pCreate->lpCreateParams);
			// Window32 ���鿡�� �ڵ鿡 ���� ������ ����Ǵµ�
			// Window32 ���鿡�� �ڵ鿡���� GWLP_USERDATA �� ���������͸� �����Ѵ�, ���� �����͸� LONG_PTR�� ����ȯ�Ͽ� �����Ѵ�.
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
			// WNDPROC ���� ����.
			SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&WindowsWindow::MsgThunk));

			return pWnd->MsgThunk(hWnd, msg, wParam, lParam);
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	LRESULT WindowsWindow::MsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		// Window32 ���鿡�� �ڵ鿡 ����� ���������͸� ���´�.
		WindowsWindow* const pWnd = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		// �ش� �����Ϳ����� �޽��� ó�� �Լ��� �ҷ��´�.
		return pWnd->MsgHandle(hWnd, msg, wParam, lParam);
	}
	LRESULT WindowsWindow::MsgHandle(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		//�޽����� ó���Ѵ�. �� Ŭ�������� �ٸ� ó������� ������.
		switch (msg)
		{
		case WM_CLOSE:
		{
			windowOpen = false;
			WindowCloseEvent event;
			m_data.EventCallback(event);
			PostQuitMessage(0);
			break;
		}
			// ������ ��Ŀ���� �Ҿ�����
		case WM_KILLFOCUS:
			/************************	Ű���� �޽���	************************/
			//  WM_KEY ���� �޽����ϰ�� wParam�� �ش� Ű������ �ڵ尡 �ִ�.
			break;
		case WM_SIZE:
		{
			if (wParam != SIZE_MINIMIZED)
			{
				UINT width = LOWORD(lParam);
				UINT height = HIWORD(lParam);
				if (pGfx != nullptr)
				{
					SetWindowSize(width, height);
				}
				if (m_data.EventCallback)
				{
					WindowResizeEvent event(width, height);
					m_data.EventCallback(event);
				}
			}
			break;
		}
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			if (wParam == VK_PROCESSKEY)
				break;
			if (wParam == VK_HANJA)
				break;

			const int scancode = (lParam >> 16) & 0x1ff;
			int key = static_cast<unsigned char>(wParam);
			int extended = (lParam & 0x01000000);
			switch (key)
			{
			case VK_SHIFT:
			{
				key = extended ? VK_RSHIFT : VK_LSHIFT;
				break;
			}
			case VK_CONTROL:
				key = extended ? VK_RCONTROL : VK_LCONTROL;
				break;
			case VK_MENU:
				key = extended ? VK_RMENU : VK_LMENU;
				break;
			}
			const int action = keyboard.OnKeyInput(key,((lParam >> 31) & 1) ? QCAT_RELEASE : QCAT_PRESS);
			
			switch (action)
			{
			if (m_data.EventCallback)
			{
			case QCAT_PRESS:
			{
				KeyPressedEvent event(key, 0);
				m_data.EventCallback(event);
				break;
			}
			case QCAT_RELEASE:
			{
				KeyReleasedEvent event(key);
				m_data.EventCallback(event);
				break;
			}
			case QCAT_REPEAT:
				KeyPressedEvent event(key, 1);
				m_data.EventCallback(event);
				break;
			}
			}	
			break;
		}
			//  WM_CHAR �ϰ�� �ش� Ű���忡 �ش��ϴ� ���ڿ��� wParam�� �ִ�.
		case WM_CHAR:
		{
			KeyTypedEvent event((int)wParam);
			m_data.EventCallback(event);
			break;
		}
			/*******************************************************************/
			/************************	���콺 �޽���	************************/
		case WM_MOUSEMOVE:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.SetPos(pt.x, pt.y);
			MouseMoveEvent event((float)pt.x, (float)pt.y);
			m_data.EventCallback(event);
			break;
		}
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		{
			int i,button, action;
			if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP)
				button = QCAT_MOUSE_BUTTON_LEFT;
			else if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP)
				button = QCAT_MOUSE_BUTTON_RIGHT;
			else if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP)
				button = QCAT_MOUSE_BUTTON_MIDDLE;
			else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
				button = QCAT_MOUSE_BUTTON_4;
			else
				button = QCAT_MOUSE_BUTTON_5;

			if (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN ||
				msg == WM_MBUTTONDOWN || msg == WM_XBUTTONDOWN)
			{
				action = QCAT_PRESS;
			}
			else
				action = QCAT_RELEASE;

			for (i = 0; i <= QCAT_MOUSE_BUTTON_LAST; i++)
			{
				if (mouse.mouseButtons[i] == QCAT_PRESS)
					break;
			}
			if (i > QCAT_MOUSE_BUTTON_LAST)
				SetCapture(hWnd);

			mouse.InputMouseClick(button, action);

			for (i = 0; i <= QCAT_MOUSE_BUTTON_LAST; i++)
			{
				if (mouse.mouseButtons[i] == QCAT_PRESS)
					break;
			}
			if (i > QCAT_MOUSE_BUTTON_LAST)
				ReleaseCapture();

			switch (action)
			{
			case QCAT_PRESS:
			{
				MouseButtonPressedEvent event(button);
				m_data.EventCallback(event);
				break;
			}
			case QCAT_RELEASE:
			{
				MouseButtonReleasedEvent event(button);
				m_data.EventCallback(event);
				break;
			}
			}
			break;
		}
		case WM_MOUSEWHEEL:
		{
			MouseScrollEvent event((float)(-((SHORT)HIWORD(wParam) / (double)WHEEL_DELTA)), 0);
			m_data.EventCallback(event);
			break;
		}
		/*******************************************************************/
		/************************ RAW ���콺 �޽��� ************************/
		case WM_INPUT:
		{
			break;
		}
		/*******************************************************************/
		}
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	std::optional<int>  WindowsWindow::UpdateMessage() noexcept
	{
		MSG msg;

		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				return (int)msg.wParam;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return {};
	}
	HWND WindowsWindow::GetHandle() noexcept
	{
		return hWnd;
	}
	void WindowsWindow::SetICon(HICON ico)
	{
		SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)ico);
		SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)ico);
	}
	void WindowsWindow::OnBegin()
	{
		const auto ecode = UpdateMessage();
		pGfx->BeginFrame();
	}
	void WindowsWindow::OnEnd()
	{
		pGfx->EndFrame();
	}
	unsigned int WindowsWindow::GetWidth() const
	{
		return width;
	}
	unsigned int WindowsWindow::GetHeight() const
	{
		return height;
	}
	void WindowsWindow::SetVSync(bool enabled)
	{
	}
	void WindowsWindow::SetWindowSize(unsigned int width,unsigned  int height) 
	{
		this->width = width;
		this->height = height;
		pGfx->CleanRenderTarget();
		pGfx->GetSwapChain()->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
		pGfx->CreateRenderTarget();
		pGfx->SetViewPort(width, height);
	}
	bool WindowsWindow::IsVSync() const
	{
		return false;
	}
}
