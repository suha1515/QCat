#include "qcpch.h"
#include "StringUtil.h"

std::vector<std::string> SplitString(const std::string& s, const std::string& delim)
{
	std::vector<std::string> strings;
	SplitStringIter(s, delim, std::back_inserter(strings));
	return strings;
}
