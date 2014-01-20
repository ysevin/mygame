#pragma once

#include <vector>

namespace SEUtil
{
	typedef std::vector<std::string> StrVec;
	std::vector<std::string> splite(const char* _str, const char* _delim = " ,");

};