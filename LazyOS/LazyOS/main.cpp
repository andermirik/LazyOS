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
	LazyOS os;

	string line;
	while (true) {
		std::getline(std::cin, line);

		auto[command, args] = parse_line(line);
		if (command == "init")
			if (!args.empty())
				os.resize(std::stoi(args[0]) << 20, 512);
			else
				os.resize(512 << 20, 512);

		cout << "command: " << command<< endl << "args: " << util::join(args, ", ") << endl;

	}

	return(0);
}
