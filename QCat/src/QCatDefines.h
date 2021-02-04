#pragma once
#define QCAT_FAIL -1
#define QCAT_RELEASE 0
#define QCAT_PRESS 1
#define QCAT_REPEAT 2

#if defined (QCAT_PLATFORM_WINDOWS)


#else

#endif

#define QCAT_DX11
//#define QCAT_OPENGL

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#if defined(QCAT_DX11)
#elif defined(QCAT_OPENGL)
#endif
