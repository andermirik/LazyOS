#include "utils.h"
#include <sstream>
namespace util {
	std::string join(std::vector<std::string> v, std::string separator) {
		std::stringstream ss;
		for (size_t i = 0; i < v.size(); ++i)
		{
			if (i != 0)
				ss << separator;
			ss << v[i];
		}
		return ss.str();
	}

	std::vector<std::string> split(const std::string& s, char seperator)
	{
		std::vector<std::string> output;
		std::string::size_type prev_pos = 0, pos = 0;
		while ((pos = s.find(seperator, pos)) != std::string::npos)
		{
			std::string substring(s.substr(prev_pos, pos - prev_pos));
			output.push_back(substring);
			prev_pos = ++pos;
		}
		output.push_back(s.substr(prev_pos, pos - prev_pos)); // Last word
		return output;
	}

	std::string replace_all(std::string str, const std::string& from, const std::string& to) {
		if (from.empty())
			return "";
		size_t start_pos = 0;
		while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
		}
		return str;
	}

	uint16_t write_first_4_bits(uint16_t mode, uint8_t num)
	{
		mode |= (num & 0x0F)<<12;
		return mode;
	}
	uint16_t read_first_4_bits(uint16_t mode)
	{

		return (mode & ((0x0F) << 12))>>12;
	}
	uint16_t write_rwxrwxrwx(uint16_t mode, uint8_t num)
	{
		mode |= (num &0x1FF);
		return mode;
	}
}