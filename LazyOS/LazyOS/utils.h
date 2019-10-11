#pragma once
#include <string>
#include <vector>
namespace util {
	std::string join(std::vector<std::string> v, std::string separator);
	std::vector<std::string> split(const std::string& s, char seperator);
	std::string replace_all(std::string str, const std::string& from, const std::string& to);
}