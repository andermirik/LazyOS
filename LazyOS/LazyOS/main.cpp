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
	//LazyOS::inode inode = { 0 };
	//inode.uid = 0xAFBEADDE;
	//GV::os.write_inode(0, inode);
	//inode.uid = 0xAFBEADDE;
	//GV::os.write_inode(1, inode);
	//GV::os.read_inode(0);
	//GV::os.get_free_block();
	//GV::os.load_root();

	
	core::fcreate("/some_file3/some/");
	uint16_t mode=0;
	util::write_first_4_bits(mode, 1);
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
