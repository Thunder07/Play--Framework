#include "StringUtils.h"

#include <sstream>

void StringUtils::replace_all(std::string& str, const std::string replace, std::string replaceWith)
{
	auto str2 = str;
	std::string::size_type pos = 0;
	while((pos = str2.find(replace, pos)) != std::string::npos)
	{
		str2 = str2.substr(0, replace.length())
		+ replaceWith
		+ str2.substr(replaceWith.length());
		pos += replaceWith.length();
	}
	str.clear();
	str += str2;
}

void StringUtils::erase_all(std::string& str, std::string remove)
{
	StringUtils::replace_all(str, remove, "");
}

void StringUtils::split(std::vector<std::string>& ret, std::string text, const char* delim, bool trim)
{
	std::istringstream iss(text);
	std::vector<std::string> results(std::istream_iterator<std::string>{iss},
									std::istream_iterator<std::string>());
}