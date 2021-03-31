#pragma once
#include "QCat/Core/PlatformDetection.h"

#ifdef QCAT_PLATFORM_WINDOWS
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <optional>

#include <bitset>
#include <queue>
#include <array>
#include <string>
#include <sstream>
#include <ostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <wrl.h>

#include "QCat/Core/QCatCore.h"
#include "QCat/Core/Log.h"
#include "QCat/Core/Assert.h"
#include "QCatDefines.h"

#include "QCat/Debug/Instrumentor.h"

#include<glm/glm.hpp>



#ifdef QCAT_PLATFORM_WINDOWS
	#include <Windows.h>
#endif

