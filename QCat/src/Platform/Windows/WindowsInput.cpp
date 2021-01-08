#include "qcpch.h"
#include "QCat/Core/Input.h"
#include "QCat/Core/Application.h"

#include "WindowsWindow.h"

namespace QCat
{
	bool Input::IsKeyPressed(int keycode)
	{
		auto window = static_cast<WindowsWindow*>(Application::GetInstance().GetWindow()->GetNativeWindow());
		auto state = window->GetKeyState(keycode);
		return state == QCAT_PRESS || state == QCAT_REPEAT;
	}

	bool Input::IsMouseButtonPressed(int button)
	{
		auto window = static_cast<WindowsWindow*>(Application::GetInstance().GetWindow()->GetNativeWindow());
		auto state = window->GetMouseState(button);
		return state == QCAT_PRESS;
	}

	std::pair<float, float> Input::GetMousePos()
	{
		auto window = static_cast<WindowsWindow*>(Application::GetInstance().GetWindow()->GetNativeWindow());
		return window->GetMousePos();
	}
	float Input::GetMouseX()
	{
		auto [x, y] = GetMousePos();
		return x;
	}

	float Input::GetMouseY()
	{
		auto [x, y] = GetMousePos();
		return y;
	}
}

