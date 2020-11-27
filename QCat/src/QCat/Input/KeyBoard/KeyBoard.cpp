#include "qcpch.h"
#include "KeyBoard.h"

namespace QCat
{
	int Keyboard::OnKeyInput(unsigned char keycode, int action)
	{
		// Ű�ڵ�� ���ȸ�ŭ�� �޴´�.
		if (keycode >= 0 && keycode <= nKeys)
		{
			bool repeat = false;
			if (action == QCAT_RELEASE && keystates[keycode] == QCAT_RELEASE)
				return action;

			if (action == QCAT_PRESS && keystates[keycode] == QCAT_PRESS)
				repeat = true;
			if (repeat)
				return QCAT_REPEAT;

			if (action == QCAT_PRESS && keystates[keycode] == QCAT_RELEASE)
			{
				OnKeyPressed(keycode);
				return QCAT_PRESS;
			}
			if (action == QCAT_RELEASE && keystates[keycode] == QCAT_PRESS)
			{
				OnKeyReleased(keycode);
				return QCAT_RELEASE;
			}
		}
		return -1;
	}
	void Keyboard::OnKeyPressed(unsigned char keycode) noexcept
	{
		keystates[keycode] = true;
	}

	void Keyboard::OnKeyReleased(unsigned char keycode) noexcept
	{
		keystates[keycode] = false;
	}

}
