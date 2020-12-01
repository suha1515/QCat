#include "qcpch.h"
#include "KeyBoard.h"

namespace QCat
{
	Keyboard::Keyboard()
	{
		keys.assign(nKeys, QCAT_RELEASE);
	}
	int Keyboard::OnKeyInput(unsigned char keycode, int action)
	{
		// 키코드는 허용된만큼만 받는다.
		if (keycode >= 0 && keycode <= nKeys)
		{
			bool repeat = false;
			if (action == QCAT_RELEASE && keys[keycode] == QCAT_RELEASE)
				return action;

			if (action == QCAT_PRESS && keys[keycode] == QCAT_PRESS)
				repeat = true;
			keys[keycode] = (char)action;

			if (repeat)
				action = QCAT_REPEAT;

			return action;
		}
		return QCAT_FAIL;
	}
	void Keyboard::OnKeyPressed(unsigned char keycode) noexcept
	{
		keystates[keycode] = true;
	}

	void Keyboard::OnKeyReleased(unsigned char keycode) noexcept
	{
		keystates[keycode] = false;
	}

	unsigned int Keyboard::GetKey(unsigned int keycode) noexcept
	{
		if (keycode >= 0 && keycode <= nKeys)
		{
			return keys[keycode];
		}
		else
			return QCAT_FAIL;
	}

}
