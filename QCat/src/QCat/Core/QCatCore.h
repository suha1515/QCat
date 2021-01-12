#pragma once
#include <memory>
#include "PlatformDetection.h"

#ifdef QCAT_DEBUG
	#define QCAT_ENABLE_ASSERTS
#endif
#ifdef QCAT_ENABLE_ASSERTS
#define QCAT_ASSERT(x, ...) { if(!(x)) { QCAT_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define QCAT_CORE_ASSERT(x, ...) { if(!(x)) { QCAT_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define QCAT_ASSERT(x, ...)
#define QCAT_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1<<x)

#define BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace QCat
{
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T,typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}
	
	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T,typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	
}