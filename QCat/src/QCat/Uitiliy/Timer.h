#pragma once
#include "QCat/QCatCore.h"
#include <chrono>

namespace QCat
{
	class QCAT_API Timer
	{
	public:
		Timer() noexcept;
		float Mark() noexcept;
		float Peek() const noexcept;
	private:
		std::chrono::steady_clock::time_point last;
	};
}
