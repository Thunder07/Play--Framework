#include "StringUtils.h"

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
