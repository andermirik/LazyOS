#include "utils.h"
#include "core.h"
#include "os.h"
#include <iostream>
#include <bitset>

using std::string;
using std::cout;
using std::endl;

std::tuple<string, std::vector<string>> parse_line(string & line) {
	
	string command = line.substr(0, line.find(' '));
	std::vector<string> args;
	if (command.size() != line.size())
		 args = util::split(line.substr(command.size() + 1), ' ');

	return std::make_tuple(command, args);
}

int main() {


	
	std::cout << core::fcreate("/some_file1/") << std::endl;
	std::cout << core::fcreate("/some_file1/dir2/") << std::endl;
	std::cout << core::fcreate("/some_file1/dir3/") << std::endl;
	std::cout << core::fcreate("/some_file1/dir2/file2/") << std::endl;
	std::cout << core::fcreate("/some_file1/dir3/file2/") << std::endl;
	std::cout << core::fcreate("/some_file1/dir3/file2") << std::endl;
	std::cout << core::fcreate("/some_file1/dir3/file") << std::endl;
	std::cout << "open:" << std::endl;
	std::cout << core::fopen("/some_file1/") << std::endl;
	std::cout << core::fopen("/some_file1/dir2/") << std::endl;
	std::cout << core::fopen("/some_file1/dir3/") << std::endl;
	std::cout << core::fopen("/some_file1/dir2/file2/") << std::endl;
	std::cout << core::fopen("/some_file1/dir3/file2/") << std::endl;
	std::cout << core::fopen("/some_file1/dir3/file") << std::endl;
	std::cout << core::fopen("/some_file1/") << std::endl<<"\n";
	std::cout << "delete:" << std::endl;
	//std::cout << core::fdelete("/some_file1/") << std::endl;
	//std::cout << core::fdelete("/some_file1/dir3/file2/")<<std::endl;
	//std::cout << core::fdelete("/some_file1/dir3/file") << std::endl;
	//std::cout << core::fdelete("/some_file1/dir3/file") << std::endl;

	char zero[256] = { 0 };
	char buf[256] = {1,2,3,4,5,0};
	char new_buf[256] = {0};

	

	//std::cout << core::fwrite(core::fopen("/some_file1/dir3/file"), 0, 256, zero);
	//std::cout << core::fwrite(core::fopen("/some_file1/dir3/file"), 0, 3, buf);
	//std::cout << core::fread(core::fopen("/some_file1/dir3/file"), 0, 256, new_buf);

	string line;
	while (true) {
		std::getline(std::cin, line);

		auto[command, args] = parse_line(line);
		if (command == "init")
			if (!args.empty())
				GV::os.resize(std::stoi(args[0]) << 20, 512);
			else
				GV::os.resize(512 << 20, 512);
		if (command == "exit")
			return 0;
		cout << "command: " << command<< endl << "args: " << util::join(args, ", ") << endl;

	}

	return(0);
}
