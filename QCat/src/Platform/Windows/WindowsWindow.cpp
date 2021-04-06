#include "qcpch.h"
#include "WindowsWindow.h"

#include "API/DirectX11/QGfxDeviceDX11.h"
#include "API//Opengl/QCatOpengl.h"

#include "QCat/Events/ApplicationEvent.h"
#include "QCat/Events/MouseEvent.h"
#include "QCat/Events/KeyboardEvent.h"

#include "backends/imgui_impl_win32.h"

#include "QCat/Core/Log.h"

#include "QCat/Renderer/RenderAPI.h"

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
namespace QCat
{
	Window* Window::Create(const WindowProps& props)
	{
		int width = props.Width;
		int height = props.Height;
		const char* title = props.Title.c_str();
		return new WindowsWindow(width, height, title);
	}

	WindowsWindow::WindowsWindow(int width, int height, const char* pWndName)
		:wndClassName(pWndName), wndName(pWndName), width(width), height(height), windowOpen(true)
	{

		QCAT_PROFILE_FUNCTION();
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
		//���� ����� ��Ÿ���� �����Ͽ� RECT �� ��������� ũ�⸦ ����Ѵ�.

		wr.left = 0;
		wr.right = width + wr.left;
		wr.top = 0;
		wr.bottom = height + wr.top;
		if ((AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE) == 0))
		{

		}
		//������ ����
		{
			QCAT_PROFILE_SCOPE("CreateWindow");
			hWnd = CreateWindow(wndClassName, wndName,
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
				nullptr, nullptr, hInstance, this
			);
		}
		// ����üũ
		if (hWnd == nullptr)
		{
			QCAT_CORE_ASSERT(false, "Create Window Failed!");
		}
		ShowWindow(hWnd, SW_SHOWDEFAULT);
		UpdateWindow(hWnd);


		if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
		{
			pGfx = std::make_unique<QGfxDeviceDX11>();
		}
		else if (RenderAPI::GetAPI() == RenderAPI::API::OpenGL)
		{
			pGfx = std::make_unique<QCatOpengl>();
		}
		

		pGfx->Init(hWnd);

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
			QCAT_CORE_WARN("Raw input device isnt set");
		}
	
	}
	WindowsWindow::~WindowsWindow()
	{
		QCAT_PROFILE_FUNCTION();
		// DLL�� ��� �� �����Ҹ� �ؾ��Ѵ�.
		UnregisterClass(wndClassName, hInstance);
		// �ڵ��� �ı��Ѵ�.
		DestroyWindow(hWnd);
	}
	Graphics* WindowsWindow::Gfx()
	{
		return pGfx.get();
	}
	LRESULT WindowsWindow::MsgSetUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		// �ʱ� �����찡 �����ɶ�
		if (msg == WM_NCCREATE)
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
		ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
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
		case WM_ACTIVATE:
		{
				//RECT rect;
				//GetClientRect(hWnd, &rect);
				////������ �������� ��ũ�� �������� ����Ʈ�� ��ȯ�Ѵ�.
				//MapWindowPoints(hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
				////api �Լ��� �������������� ���콺��ġ�� �����Ѵ�.
				////������ Ŭ���̾�Ʈ ������ŭ ���ѵȴ�.
				//ClipCursor(&rect);
		}
			break;
		// ������ ��Ŀ���� �Ҿ�����
		case WM_CREATE:
			
			break;
		case WM_KILLFOCUS:
			break;
		case WM_SIZE:
		{
			if (wParam != SIZE_MINIMIZED)
			{
				UINT width = LOWORD(lParam);
				UINT height = HIWORD(lParam);
				SetWindowSize(width, height);
				if (m_data.EventCallback)
				{
					WindowResizeEvent event(width, height);
					m_data.EventCallback(event);
				}
			}
			break;
		}
		/************************	Ű���� �޽���	************************/
		//  WM_KEY ���� �޽����ϰ�� wParam�� �ش� Ű������ �ڵ尡 �ִ�.
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
			const int action = keyboard.OnKeyInput(key, ((lParam >> 31) & 1) ? QCAT_RELEASE : QCAT_PRESS);

			switch (action)
			{
				if (m_data.EventCallback)
				{
			case QCAT_PRESS:
			{
				KeyPressedEvent event(key, 0);
				m_data.EventCallback(event);
				//std::cout << event.GetKeyCode()<<" key Pressed" << std::endl;
				break;
			}
			case QCAT_RELEASE:
			{
				KeyReleasedEvent event(key);
				m_data.EventCallback(event);
				//std::cout << event.GetKeyCode() << " key Released " << std::endl;
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
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		{
			int i, button, action;
			
			if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP)
				button = Mouse::ButtonLeft;
			else if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP)
				button = Mouse::ButtonRight;
			else if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP)
				button = Mouse::ButtonMiddle;
			else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
				button = Mouse::Button3;
			else
				button = Mouse::Button4;

			if (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN ||
				msg == WM_MBUTTONDOWN || msg == WM_XBUTTONDOWN)
			{
				action = QCAT_PRESS;
			}
			else
				action = QCAT_RELEASE;

			for (i = 0; i <= Mouse::ButtonLast; i++)
			{
				if (mouse.mouseButtons[i] == QCAT_PRESS)
					break;
			}
			if (i > Mouse::ButtonLast)
				SetCapture(hWnd);

			mouse.InputMouseClick(button, action);

			for (i = 0; i <= Mouse::ButtonLast; i++)
			{
				if (mouse.mouseButtons[i] == QCAT_PRESS)
					break;
			}
			if (i > Mouse::ButtonLast)
				ReleaseCapture();

			switch (action)
			{
			case QCAT_PRESS:
			{
				MouseButtonPressedEvent event(button);
				m_data.EventCallback(event);
				//std::cout << event.GetMouseButton() << " key Pressed" << std::endl;
				break;
			}
			case QCAT_RELEASE:
			{
				MouseButtonReleasedEvent event(button);
				m_data.EventCallback(event);
				//std::cout << event.GetMouseButton() << " key Released" << std::endl;
				break;
			}
			}
			break;
		}
		case WM_MOUSEWHEEL:
		{
			MouseScrollEvent event(0.0f, float((SHORT)HIWORD(wParam) / (double)WHEEL_DELTA));
			m_data.EventCallback(event);
			break;
		}
		/*******************************************************************/
		/************************ RAW ���콺 �޽��� ************************/
		case WM_INPUT:
		{
			UINT size;
			// get input data size using GetRawInputData
			// 3rd parameter is nullptr for not getting real data only get data size
			if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam),
				RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER)) == -1)
				break;

			// make buffer with size
			rawBuffer.resize(size);
			// read Data
			if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam),
				RID_INPUT, rawBuffer.data(), &size, sizeof(RAWINPUTHEADER)) == -1)
				break;

			// casting to RAWINPUT 
			auto& ri = reinterpret_cast<const RAWINPUT&>(*rawBuffer.data());

			// Header has a typeinfo so there is may rawinputdata we need to distringuishi
			// if we nned mousetype info use RIM_TYPEMOUSE

			if (ri.header.dwType == RIM_TYPEMOUSE)
			{
				int wheelDelta = 0;
				if (ri.data.mouse.usButtonFlags & RI_MOUSE_WHEEL)
				{
					wheelDelta = (*(short*)&ri.data.mouse.usButtonData) / WHEEL_DELTA;
				}
				mouse.OnRawDelta(ri.data.mouse.lLastX, ri.data.mouse.lLastY, wheelDelta);
			}

			break;
		}
		/*******************************************************************/
		}
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	bool WindowsWindow::OnMessageUpdate()
	{
		MSG msg;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				return false;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return true;
	}
	std::optional<int>  WindowsWindow::UpdateMessage() noexcept
	{
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
		pGfx->Begin();
	}
	void WindowsWindow::OnEnd()
	{
		pGfx->End();
	}
	uint32_t WindowsWindow::GetWidth() const
	{
		return width;
	}
	uint32_t WindowsWindow::GetHeight() const
	{
		return height;
	}
	void WindowsWindow::SetVSync(bool enabled)
	{
	}
	void WindowsWindow::SetWindowSize(unsigned int width, unsigned  int height)
	{

		this->width = width;
		this->height = height;
	}
	bool WindowsWindow::IsVSync() const
	{
		return false;
	}
}
