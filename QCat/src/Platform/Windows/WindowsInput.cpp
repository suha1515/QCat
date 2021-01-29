#include "qcpch.h"
#include "QCat/Core/Input.h"
#include "QCat/Core/Application.h"

#include "WindowsWindow.h"


namespace QCat
{
	bool Input::IsKeyPressed(const KeyCode keycode)
	{
		auto* window = static_cast<WindowsWindow*>(Application::GetInstance().GetWindow()->GetNativeWindow());
		auto state = window->GetKeyState(keycode);
		return state == QCAT_PRESS || state == QCAT_REPEAT;
	}

	bool Input::IsMouseButtonPressed(const MouseCode button)
	{
		auto* window = static_cast<WindowsWindow*>(Application::GetInstance().GetWindow()->GetNativeWindow());
		auto state = window->GetMouseState(button);
		return state == QCAT_PRESS;
	}

	glm::vec2 Input::GetMousePos()
	{
		auto* window = static_cast<WindowsWindow*>(Application::GetInstance().GetWindow()->GetNativeWindow());
		double xpos, ypos;
		std::pair<int, int> pair = window->GetMousePos();
		xpos = pair.first;
		ypos = pair.second;
		return { (float)xpos,(float)ypos };
	}
	float Input::GetMouseX()
	{
		return  GetMousePos().x;
	}

	float Input::GetMouseY()
	{
		return  GetMousePos().y;
	}
	std::optional<MouseDevice::RawDelta> Input::GetDeltaData()
	{
		auto* window = static_cast<WindowsWindow*>(Application::GetInstance().GetWindow()->GetNativeWindow());
		auto info = window->GetMouseRawDelta();
		return info;
	}
}

