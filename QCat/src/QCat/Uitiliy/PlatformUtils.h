#pragma once

#include<string>

namespace QCat
{
	class FileDialogs
	{
	public:
		// Return empty string if cancelled
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);

	};
}