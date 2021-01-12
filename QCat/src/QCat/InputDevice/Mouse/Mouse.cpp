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
}

