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

#define BIT(x) (1<<x)