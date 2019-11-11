#include "utils.h"
#include "core.h"
#include "os.h"
#include <iostream>
#include <bitset>
#include "windows.h"

#include <map>
#include <functional>

using std::string;
using std::cout;
using std::cin;
using std::endl;

std::tuple<string, std::vector<string>> parse_line(string & line) {
	
	string command = line.substr(0, line.find(' '));
	std::vector<string> args;
	if (command.size() != line.size())
		 args = util::split(line.substr(command.size() + 1), ' ');

	return std::make_tuple(command, args);
}

int main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	setlocale(0, "rus");

	std::map<std::string, std::function<void(std::vector<string>)>> cmds;

	cmds["init"] = [](std::vector<string> args) {
		if (!args.empty())
			GV::os.resize(std::stoi(args[0]) << 20, 512);
		else
			GV::os.resize(512 << 20, 512);
	};

	cmds["exit"] = [](std::vector<string> args) {
		exit(0);
	};

	cmds["clear"] = [](std::vector<string> args) {
		system("cls");
	};

	cmds["users"] = [](std::vector<string> args) {
		if (!args.empty()) {
			if (args[0] == "add") {
				string login, pswd;
				if (args.size() > 1) {
					login = args[1];
				}
				else {
					cout << "логин: ";
					std::getline(cin, login);
				}
				cout << "пароль: ";
				pswd = util::read_pswd();

				int uid = GV::os.user_add(login, pswd);
				if (uid != -1) {
					cout << "пользователь " << login << " с uid " << uid << " успешно добавлен." << endl;
				}
				else {
					cout << "во время добавления пользователя возникла ошибка." << endl
						 << "проверьте логин еще раз!" << endl;
				}
			}
			else if (args[0] == "get") {
				cout << "список пользователей: " << endl;

				for (auto& user : GV::os.user_get()) {
					printf("uid: %5d\tlogin: %s\n", user.first, user.second.c_str());
				}

			}
			else if (args[0] == "delete") {
				string login, pswd;
				if (args.size() > 1) {
					login = args[1];
				}
				else {
					cout << "логин: ";
					std::getline(cin, login);
				}
				cout << "пароль: ";
				pswd = util::read_pswd();

				int uid = GV::os.user_login(login, pswd);
				if (uid != -1) {
					GV::os.user_rmv(login);
					cout << "пользователь " << login << " успешно удалён!" << endl;
				}
				else {
					cout << "не верный логин или пароль" << endl;
				}
			}
			else if (args[0] == "rename") {
				string login, pswd, new_login;
				if (args.size() > 1) {
					login = args[1];
				}
				else {
					cout << "логин: ";
					std::getline(cin, login);
				}
				cout << "пароль: ";
				pswd = util::read_pswd();

				cout << "новый логин: ";
				std::getline(cin, new_login);

				int uid = GV::os.user_login(login, pswd);
				if (uid != -1) {
					GV::os.user_rnm(login, new_login);
					cout << "пользователь " << login << " успешно переименован!" << endl;
				}
				else {
					cout << "не верный логин или пароль" << endl;
				}
			}
			else if (args[0] == "login") {

			}
		}
	};

	string line;
	while (true) {
		cout << "$ ";
		std::getline(std::cin, line);

		auto[command, args] = parse_line(line);
		
		bool worked = false;
		for (auto& it : cmds) {
			if (it.first == command) {
				worked = true;
				it.second(args);
				break;
			}
		}
		if (!worked) {
			cout << "Не найдена комманда " << command << endl;
		}

	}

	return(0);
}
