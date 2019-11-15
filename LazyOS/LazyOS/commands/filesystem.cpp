#include "../globals.h"
#include "../core.h"
#include "../utils.h"
#include <iostream>
#include <bitset>
using std::string;
using std::cout;
using std::cin;
using std::endl;

void set_filesystem_commands() {
	GV::cmds["dir"] = [](std::vector<std::string> args) {
		if (args.empty()) {
			args.push_back(util::join(GV::os.dirs, "/"));
		}
		int file_inode = core::fopen(args[0]);
		if (file_inode == 0 && args[0] != "/") {
			cout << "такого файла не существует!" << endl;
			return;
		}
		int size = core::fsize(file_inode);
		char* buf = new char[size];
		core::fread(file_inode, 0, size, buf);
		for (int i = 0; i < size / 64; i++) {
			LazyOS::directory_file file;
			memcpy(&file, buf + i * 64, 64);

			LazyOS::inode inode = GV::os.read_inode(file.n_inode);
			uint16_t attrs = core::fget_attributes(file.n_inode);
			uint16_t type = util::read_first_4_bits(attrs);
			uint16_t rules = util::read_rwxrwxrwx(attrs);

			if (type == 0x5D || type == 0xD) {
				util::set_text_color(colors::Yellow);
				cout << util::file_to_filename(file) << "/";
				util::set_text_color(colors::White);
			}
			else {
				cout << util::file_to_filename(file);
			}

			cout << " ";

			std::bitset<9> rwx(rules);
			for (int i = 0; i < 9; i++) {
				if (rwx[i] == 1) {
					if (i % 3 == 0)
						cout << "r";
					else if (i % 3 == 1)
						cout << "w";
					else if (i % 3 == 2)
						cout << "x";
				}
				else {
					cout << "-";
				}
			}
			cout << " uid: " << inode.uid << endl;
		}
		
	};
	GV::cmds["cd"] = [](std::vector<std::string> args) {
		if (!args.empty()) {
			if (args[0] == "") {
				return;
			}
			
			string path = GV::os.relative_to_full_path(args[0]);

			int file_inode = core::fopen(path);
			int file_type = util::read_first_4_bits(core::fget_attributes(file_inode));
			bool is_dir = file_type == 0xD || file_type == 0x5D;
			
			if ((file_inode || path == "/") && is_dir == true) {
				GV::os.dirs = util::split(path, '/');
				if (file_inode != 0) {
					if (path[path.size()-1] != '/') {
						GV::os.dirs.push_back("");
					}
				}
			}
			else {
				cout << "не удалось открыть файл " << path << endl;
			}
		}
	};
	GV::cmds["mk"] = [](std::vector<std::string> args) {
		if (!args.empty()) {
			string path = GV::os.relative_to_full_path(args[0]);
			if (core::fcreate(path) == 0) {
				cout << "не удалось создать файл " << path << endl;
			}
		}
	};
	GV::cmds["rm"] = [](std::vector<std::string> args) {
		if (!args.empty()) {
			string path = GV::os.relative_to_full_path(args[0]);
			if (core::fdelete(path) == 0) {
				cout << "не удалось удалить файл " << path << endl;
			}
		}
	};
	GV::cmds["rn"] = [](std::vector < std::string> args) {
		if (args.size() > 1) {
			string path = GV::os.relative_to_full_path(args[0]);
			if (core::frename(path, args[1]) == 0) {
				cout << "не удалось переименовать файл " << path << endl;
			}
		}
	};
}