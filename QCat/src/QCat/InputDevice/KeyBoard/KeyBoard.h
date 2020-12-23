#pragma once
#include "QCat/Core/QCatCore.h"

namespace QCat
{
	class QCAT_API Keyboard
	{
	public:
		Keyboard();
		~Keyboard() = default;

		int OnKeyInput(unsigned char keycode, int action);
		void OnKeyPressed(unsigned char keycode) noexcept;
		void OnKeyReleased(unsigned char keycode) noexcept;
	public:
		unsigned int GetKey(unsigned int keycode) noexcept;
	private:
		static constexpr unsigned int nKeys = 256u;
		std::bitset<nKeys> keystates;
		std::vector<char> keys;
		std::queue<char> charbuffer;
	};
}