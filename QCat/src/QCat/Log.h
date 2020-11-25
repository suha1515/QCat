#pragma once
#include "QCatCore.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace QCat
{
	class QCAT_API Log
	{
	public:
		Log();
		~Log();
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// 코어 로그 매크로
#define QCAT_CORE_TRACE(...)	::QCat::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define QCAT_CORE_INFO(...)		::QCat::Log::GetCoreLogger()->info(__VA_ARGS__)
#define QCAT_CORE_WARN(...)		::QCat::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define QCAT_CORE_ERROR(...)	::QCat::Log::GetCoreLogger()->error(__VA_ARGS__)
#define QCAT_CORE_FATAL(...) 	::QCat::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// 클라이언트 로그 매크로
#define QCAT_TRACE(...)	::QCat::Log::GetClientLogger()->trace(__VA_ARGS__)
#define QCAT_INFO(...)		::QCat::Log::GetClientLogger()->info(__VA_ARGS__)
#define QCAT_WARN(...)		::QCat::Log::GetClientLogger()->warn(__VA_ARGS__)
#define QCAT_ERROR(...)	::QCat::Log::GetClientLogger()->error(__VA_ARGS__)
#define QCAT_FATAL(...) 	::QCat::Log::GetClientLogger()->fatal(__VA_ARGS__)

