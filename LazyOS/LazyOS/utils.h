#pragma once
#include <string>
#include <vector>
namespace util {
	std::string join(std::vector<std::string> v, std::string separator);
	std::vector<std::string> split(const std::string& s, char seperator);
	std::string replace_all(std::string str, const std::string& from, const std::string& to);

	uint16_t write_first_4_bits(uint16_t mode, uint8_t num);
	uint16_t write_rwxrwxrwx(uint16_t mode, uint8_t num);
}