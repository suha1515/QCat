#include "qcpch.h"
#include "WindowsWindow.h"

#include "QCat/Events/ApplicationEvent.h"
#include "QCat/Events/MouseEvent.h"
#include "QCat/Events/KeyboardEvent.h"

#include "Platform/ImGui/Windows_Directx11/imgui_impl_win32.h"

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
		//윈도우 클래스 등록
		hInstance = GetModuleHandle(nullptr);
		WNDCLASSEX wc = { 0 };
		wc.cbSize = sizeof(wc);
		wc.style = CS_OWNDC;
		wc.lpfnWndProc = MsgSetUp;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = static_cast<HICON>(LoadImage(hInstance, IDI_APPLICATION, IMAGE_ICON, 32, 32, 0));
		UINT errorcode = 0;
		errorcode = GetLastError();
		wc.hCursor = nullptr;
		wc.hbrBackground = nullptr;
		wc.lpszMenuName = nullptr;
		wc.lpszClassName = wndClassName;
		wc.hIconSm = static_cast<HICON>(LoadImage(hInstance, IDI_APPLICATION, IMAGE_ICON, 16, 16, 0));
		RegisterClassEx(&wc);

		// 윈도우 사이즈를 기준으로하여 클라이언트 영역을 계산한다.
		RECT wr;
		wr.left = 0;
		wr.right = width + wr.left;
		wr.top = 0;
		wr.bottom = height + wr.top;
		//현재 적용된 스타일을 참고하여 RECT 로 새로적용된 크기를 계산한다.
		if ((AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0))
		{
			
		}
		//윈도우 생성
		hWnd = CreateWindow(wndClassName, wndName,
			WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
			nullptr, nullptr, hInstance, this
		);
		// 에러체크
		if (hWnd == nullptr)
		{
		
		}

		ShowWindow(hWnd, SW_SHOWDEFAULT);


		pGfx = std::make_unique<QGfxDeviceDX11>(hWnd, width, height);
		// Raw값을 받아올 장치에대해 세부화한다
		// 입력장치에는 다양한 장치가있으며 사용자는 해당 장치에대해서 입력을 받고자할때
		// WM_MOUSEMOVE 나 WM_KEYDOWN 같은 키보드나 마우스 메시지가아닌 WM_INPUT으로 해당 장치에대한 정보를 받아야한다
		RAWINPUTDEVICE rid;
		rid.usUsagePage = 0x01; // mouse page
		rid.usUsage = 0x02; // mouse usage
		rid.dwFlags = 0;
		rid.hwndTarget = nullptr;	//raw 정보를 받을 윈도우핸들
		//장치등록
		if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
		{
		
		}
	}
	WindowsWindow::~WindowsWindow()
	{
		// DLL의 경우 꼭 등록취소를 해야한다.
		UnregisterClass(wndClassName, hInstance);
		// 핸들을 파괴한다.
		DestroyWindow(hWnd);
	}
	QGfxDeviceDX11& WindowsWindow::Gfx()
	{
		return *pGfx;
	}
	LRESULT WindowsWindow::MsgSetUp(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		// 초기 윈도우가 생성될때
		if (msg == WM_NCCREATE )
		{
			// CreateWindow 함수에서 전달한 마지막 파라매터 정보를 사용하기위해 형변환을 한다.
			const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
			// lParam 자료를 CREATESTRUCT 구조체로 변환후 lpCreateParams 멤버에 접근하여 this 포인터로 저장된 자료를 형변환한다.
			WindowsWindow* const pWnd = static_cast<WindowsWindow*>(pCreate->lpCreateParams);
			// Window32 측면에서 핸들에 대한 정보가 저장되는데
			// Window32 측면에서 핸들에대한 GWLP_USERDATA 즉 유저데이터를 저장한다, 받은 포인터를 LONG_PTR로 형변환하여 저장한다.
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
			// WNDPROC 새로 지정.
			SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&WindowsWindow::MsgThunk));

			return pWnd->MsgHandle(hWnd, msg, wParam, lParam);
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	LRESULT WindowsWindow::MsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		// Window32 측면에서 핸들에 저장된 유저데이터를 얻어온다.
		WindowsWindow* const pWnd = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		// 해당 포인터에대한 메시지 처리 함수를 불러온다.
		return pWnd->MsgHandle(hWnd, msg, wParam, lParam);
	}
	LRESULT WindowsWindow::MsgHandle(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		//메시지를 처리한다. 각 클래스마다 다른 처리방식을 가진다.
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
			// 윈도우 포커스를 잃었을때
		case WM_KILLFOCUS:
			/************************	키보드 메시지	************************/
			//  WM_KEY 관련 메시지일경우 wParam에 해당 키보드의 코드가 있다.
			break;
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			const int scancode = (lParam >> 16) & 0x1ff;
			const int key = static_cast<unsigned char>(wParam);
			const int action = keyboard.OnKeyInput(key,((lParam >> 31) & 1) ? QCAT_RELEASE : QCAT_PRESS);
			
			switch (action)
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
			}
			break;
		}
			//  WM_CHAR 일경우 해당 키보드에 해당하는 문자열이 wParam에 있다.
		case WM_CHAR:
			break;
			/*******************************************************************/
			/************************	마우스 메시지	************************/
		case WM_MOUSEMOVE:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			MouseMoveEvent event((float)pt.x, (float)pt.y);
			m_data.EventCallback(event);
			break;
		}
		case WM_LBUTTONDOWN:
		{
			MouseButtonPressedEvent event(0);
			m_data.EventCallback(event);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			MouseButtonPressedEvent event(1);
			m_data.EventCallback(event);
			break;
		}
		case WM_LBUTTONUP:
		{
			MouseButtonReleasedEvent event(0);
			m_data.EventCallback(event);
			break;
		}
		case WM_RBUTTONUP:
		{
			MouseButtonReleasedEvent event(1);
			m_data.EventCallback(event);
			break;
		}
		case WM_MOUSEWHEEL:
		{
			MouseScrollEvent event((float)(-((SHORT)HIWORD(wParam) / (double)WHEEL_DELTA)), 0);
			m_data.EventCallback(event);
			break;
		}
		/*******************************************************************/
		/************************ RAW 마우스 메시지 ************************/
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
	bool WindowsWindow::IsVSync() const
	{
		return false;
	}
}
