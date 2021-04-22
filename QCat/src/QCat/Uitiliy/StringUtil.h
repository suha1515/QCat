#pragma once
#include <string>
#include <vector>
template<class Iter>
void SplitStringIter(const std::string& s, const std::string& delim, Iter out)
{
	if (delim.empty())
	{
		*out++ = s;
	}
	else
	{
		size_t a = 0, b = s.find(delim);
		for (; b != std::string::npos;
			a = b + delim.length(), b = s.find(delim, a))
		{
			*out++ = std::move(s.substr(a, b - a));
		}
		*out++ = std::move(s.substr(a, s.length() - a));
	}
}

std::vector<std::string> SplitString(const std::string& s, const std::string& delim);

