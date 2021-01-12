#pragma once
#include <glm/glm.hpp>

#include "KeyCode.h"
#include "MouseCode.h"

namespace QCat
{
	class  Input
	{
	public:
		static bool IsKeyPressed(const KeyCode keycode);
		static bool IsMouseButtonPressed(const MouseCode button);
		static glm::vec2 GetMousePos();
		static float GetMouseX();
		static float GetMouseY();
	};
}

