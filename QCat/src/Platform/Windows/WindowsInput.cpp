#include "qcpch.h"
#include "WindowsInput.h"
#include "WindowsWindow.h"

#include "QCat/Application.h"


namespace QCat
{
	Input* Input::s_Instance = new WindowsInput();
	bool WindowsInput::IsKeyPressedImpl(int keycode)
	{
		auto window = static_cast<WindowsWindow*>(Application::GetInstance().GetWindow()->GetNativeWindow());
		auto state = window->GetKeyState(keycode);
		return state == QCAT_PRESS || state == QCAT_REPEAT;
	}

	bool WindowsInput::IsMouseButtonPressedImpl(int button)
	{
		auto window = static_cast<WindowsWindow*>(Application::GetInstance().GetWindow()->GetNativeWindow());
		auto state = window->GetMouseState(button);
		return state == QCAT_PRESS;
	}

	std::pair<float, float> WindowsInput::GetMousePosImpl()
	{
		auto window = static_cast<WindowsWindow*>(Application::GetInstance().GetWindow()->GetNativeWindow());
		return window->GetMousePos();
	}
	float WindowsInput::GetMouseXImpl()
	{
		auto [x, y] = GetMousePosImpl();
		return x;
	}

	float WindowsInput::GetMouseYImpl()
	{
		auto [x, y] = GetMousePosImpl();
		return y;
	}
}

