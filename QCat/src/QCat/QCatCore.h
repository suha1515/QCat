#pragma once

#ifdef QCAT_PLATFORM_WINDOWS
	#ifdef QCAT_BUILD_DLL
		#define QCAT_API __declspec(dllexport)
	#elif QCAT_BUILD_DLL_USE
		#define QCAT_API __declspec(dllImport)
	#elif QCAT_BUILD_LIB
		#define QCAT_API 
	#else 
		#define QCAT_API
	#endif
#else
	#error QCat only support Windows!
#endif

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

