#pragma once
#include "QCat/Core/QCatCore.h"
#include "QCat/Core/Log.h"
#include <filesystem>

#ifdef QCAT_ENABLE_ASSERTS


// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
#define QCAT_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { QCAT##type##ERROR(msg, __VA_ARGS__); QCAT_DEBUGBREAK(); } }
#define QCAT_INTERNAL_ASSERT_WITH_MSG(type, check, ...) QCAT_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define QCAT_INTERNAL_ASSERT_NO_MSG(type, check) QCAT_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", QCAT_STRINGFY_MACRO(check),  std::filesystem::path(__FILE__).filename().string().c_str(), __LINE__)

#define QCAT_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define QCAT_INTERNAL_ASSERT_GET_MACRO(...) QCAT_EXPAND_MACRO( QCAT_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, QCAT_INTERNAL_ASSERT_WITH_MSG, QCAT_INTERNAL_ASSERT_NO_MSG) )

// Currently accepts at least the condition and one additional parameter (the message) being optional
#define QCAT_ASSERT(...) QCAT_EXPAND_MACRO( QCAT_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
#define QCAT_CORE_ASSERT(...) QCAT_EXPAND_MACRO( QCAT_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
#define QCAT_ASSERT(...)
#define QCAT_CORE_ASSERT(...)
#endif
