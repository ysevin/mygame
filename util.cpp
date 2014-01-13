#include "StdAfx.h"

#include "util.h"

std::vector<std::string> SEUtil::splite(const char* _str, const char* _delim/* = " ,"*/)
{
	int len = strlen(_str);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	std::vector<std::string> result;

	strncpy(str, _str, len);
	char* s = strtok(str, _delim);
	while(s)
	{
		result.push_back(s);
		s = strtok(NULL, _delim);
	}
	delete[] str;

	return result;
}