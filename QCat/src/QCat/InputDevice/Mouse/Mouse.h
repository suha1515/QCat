#pragma once
#include "QCat/Core/QCatCore.h"
#include <queue>
#include <optional>

namespace QCat
{
	class QCAT_API MouseDevice
	{
		friend class WindowsWindow;
	public:
		struct RawDelta
		{
			int x; 
			int y;
			int wheelDelta;
		};
		MouseDevice();
		~MouseDevice() = default;

		void InputMouseClick(int button, int action);
		void SetPos(int x, int y) { this->x = x, this->y = y; };
		int GetButton(int button);
		std::pair<int, int> GetMousePos() { return { x,y, };};

		void OnRawDelta(int dx, int dy, int wheel) noexcept;
		void TrimRawBuffer() noexcept;
		std::optional<MouseDevice::RawDelta> ReadRawDelta() noexcept;
	public:
		int x, y;
		int wheelDeltaCarry = 0;
		std::vector<char> mouseButtons;
		std::queue<RawDelta> rawDeltaBuffer;
	};
}