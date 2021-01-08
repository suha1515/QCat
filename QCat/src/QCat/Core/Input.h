#pragma once
#include "QCat/Core/QCatCore.h"
#include "KeyCode.h"
#include <utility>

namespace QCat
{
	class QCAT_API Input
	{
	public:
		static bool IsKeyPressed(int keycode);
		static bool IsMouseButtonPressed(int button);
		static std::pair<float, float> GetMousePos();
		static float GetMouseX();
		static float GetMouseY();
	};
}

