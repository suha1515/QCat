#include "qcpch.h"
#include "Mouse.h"
#include "QCat/Core/MouseCode.h"

namespace QCat
{
	MouseDevice::MouseDevice()
	{
		mouseButtons.assign(Mouse::ButtonLast+1,QCAT_RELEASE);
	}

	void MouseDevice::InputMouseClick(int button, int action)
	{
		if (button <0 || button>Mouse::ButtonLast)
			return;

		mouseButtons[button] = (char)action;
 	}
	int MouseDevice::GetButton(int button)
	{
		if (button<0 || button>Mouse::ButtonLast)
			return QCAT_FAIL;
		
		return mouseButtons[button];
	}
	void MouseDevice::OnRawDelta(int dx, int dy, int wheel) noexcept
	{
		rawDeltaBuffer.push({ dx,dy,wheel });
		TrimRawBuffer();
	}
	void MouseDevice::TrimRawBuffer() noexcept
	{
		while (rawDeltaBuffer.size() > 16)
		{
			rawDeltaBuffer.pop();
		}
	}
	std::optional<MouseDevice::RawDelta> MouseDevice::ReadRawDelta() noexcept
	{
			if (rawDeltaBuffer.empty())
				return std::nullopt;
		const RawDelta d = rawDeltaBuffer.front();
		rawDeltaBuffer.pop();
		return d;
	}
}

