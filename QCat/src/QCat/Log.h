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

#define QCAT_CORE_ERROR(...) ::QCat::Log::GetCoreLogger()->error(__VA_ARGS__)

