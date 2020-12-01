#include "qcpch.h"
#include "Mouse.h"


namespace QCat
{
	Mouse::Mouse()
	{
		mouseButtons.assign(QCAT_MOUSE_BUTTON_LAST+1,QCAT_RELEASE);
	}

	void Mouse::InputMouseClick(int button, int action)
	{
		if (button <0 || button>QCAT_MOUSE_BUTTON_LAST)
			return;

		mouseButtons[button] = (char)action;
 	}
	int Mouse::GetButton(int button)
	{
		if (button<0 || button>QCAT_MOUSE_BUTTON_LAST)
			return QCAT_FAIL;
		
		return mouseButtons[button];
	}
}

