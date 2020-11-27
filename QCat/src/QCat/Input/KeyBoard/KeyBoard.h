#pragma once
#include "QCat/QCatCore.h"

namespace QCat
{
	class QCAT_API Keyboard
	{
	public:
		Keyboard() = default;
		~Keyboard() = default;

		int OnKeyInput(unsigned char keycode, int action);
		void OnKeyPressed(unsigned char keycode) noexcept;
		void OnKeyReleased(unsigned char keycode) noexcept;
	private:
		static constexpr unsigned int nKeys = 256u;
		std::bitset<nKeys> keystates;
		std::queue<char> charbuffer;
	};
}