#pragma once

#include<string>
#include<optional>

namespace QCat
{
	class FileDialogs
	{
	public:
		// Return empty string if cancelled
		static std::optional<std::string> OpenFile(const char* filter);
		static std::optional<std::string> SaveFile(const char* filter);

	};
}