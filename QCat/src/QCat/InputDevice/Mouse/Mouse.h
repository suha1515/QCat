#pragma once
#include "QCat/Core/QCatCore.h"

namespace QCat
{
	class QCAT_API MouseDevice
	{
	public:
		MouseDevice();
		~MouseDevice() = default;

		void InputMouseClick(int button, int action);
		void SetPos(int x, int y) { this->x = x, this->y = y; };
		int GetButton(int button);
		std::pair<int, int> GetMousePos() { return { x,y, };};
	public:
		int x, y;
		int wheelDeltaCarry = 0;
		std::vector<char> mouseButtons;
	};
}