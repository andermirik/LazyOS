#include "../globals.h"
#include "../core.h"
#include "../utils.h"
#include <iostream>
#include <bitset>
#include <chrono>
#include <sstream>
#include <iomanip>
using std::string;
using std::cout;
using std::cin;
using std::endl;



std::string long_to_time(uint64_t ltime) {
	time_t ttime = (time_t)ltime/1000;
	return std::string(ctime(&ttime));
}

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
				cout << std::setw(10) << std::left 
					<< util::file_to_filename(file) + "/";
				util::set_text_color(colors::White);
			}
			else {
				cout << std::setw(10) << std::left 
					<< util::file_to_filename(file);
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

			cout << " uid: " << inode.uid << "\t";
			cout << " gid: " << inode.gid << "\t";
			cout << " size: " << inode.size << "\t";
			cout << " time: " << util::replace_all(long_to_time(inode.date_modification), "\n", "") << "\t";

			cout << " iid: " << file.n_inode << endl;
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
	GV::cmds["mv"] = [](std::vector < std::string> args) {
		if (args.size() > 1) {
			string path = GV::os.relative_to_full_path(args[0]);
			string path2 = GV::os.relative_to_full_path(args[1]);

			int file_inode = core::fopen(path);
			if (path != "/" && file_inode) {
				int size = core::fsize(file_inode);

				char* buf;
				buf = new char[1];//чтоб не было ошибки
				if (size != 0) {
					buf = new char[size];
					core::fread(file_inode, 0, size, buf);
				}
				int new_file_inode = core::fcreate(path2);
				if (new_file_inode && path2 != "/") {
					if (size != 0) {
						core::fwrite(new_file_inode, 0, size, buf);
					}
				}
				else {
					cout << "не удалось содать файл " << path2 << endl;
				}
				core::fdelete(path);
			}
			else {
				cout << "не удалось создать файл " << path << endl;
			}
		}
	};
	GV::cmds["append"] = [](std::vector < std::string> args) {
		if (args.size()>1) {
			string path = GV::os.relative_to_full_path(args[0]);
			int size = std::stoi(args[1]);

			int file_inode = core::fopen(path);
			if (file_inode) {
				char*buf = new char[size];
				cout << "> ";
				cin.read(buf, size);
				if (std::cin.peek()) std::cin.ignore();
				cout << "дозаписано " << core::fappend(file_inode, size, buf) << " байт" << endl;
			}
			else {
				cout << "файл " << path << " не удалось открыть." << endl;
			}
		}
		else {
			cout << "недостаточно аргументов. \nструктрура комманды: append <path> <count>" << endl;
		}
	};
	GV::cmds["read"] = [](std::vector < std::string> args) {
		if (args.size() > 2) {
			string path = GV::os.relative_to_full_path(args[0]);
			int offset = std::stoi(args[1]);
			int size = std::stoi(args[2]);

			int file_inode = core::fopen(path);
			if (file_inode) {
				char*buf = new char[size];
				memset(buf, 0, size);
				core::fread(file_inode, offset, size, buf);
				cout << "прочитано: " << std::string(buf, size) << endl;
			}
			else {
				cout << "файл " << path << " не удалось открыть." << endl;
			}
		}
		else {
			cout << "недостаточно аргументов. \nструктрура комманды: read <path> <offset> <count>" << endl;
		}
	};
	GV::cmds["write"] = [](std::vector < std::string> args) {
		if (args.size() > 2) {
			string path = GV::os.relative_to_full_path(args[0]);
			int offset = std::stoi(args[1]);
			int size = std::stoi(args[2]);

			int file_inode = core::fopen(path);
			if (file_inode) {
				char*buf = new char[size];
				cout << "> ";
				cin.read(buf, size);
				if (std::cin.peek()) std::cin.ignore();
				cout << "записано: " << core::fwrite(file_inode, offset, size, buf) << " байт" << endl;
			}
			else {
				cout << "файл " << path << " не удалось открыть." << endl;
			}
		}
		else {
			cout << "недостаточно аргументов. \nструктрура комманды: write <path> <offset> <count>" << endl;
		}
	};
}