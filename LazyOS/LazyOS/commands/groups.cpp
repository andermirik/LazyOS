#include "../globals.h"
#include <iostream>
#include "../utils.h"

using std::string;
using std::cout;
using std::cin;
using std::endl;

void set_groups_commands() {
	GV::cmds["groups"] = [](std::vector<string> args) {
		if (!args.empty()) {
			if (args[0] == "add") {
				string name, pswd;
				if (args.size() > 1) {
					name = args[1];
				}
				else {
					cout << "имя группы: ";
					std::getline(cin, name);
				}
				if (name != "default") {
					cout << "пароль: ";
					pswd = util::read_pswd();

					int gid = GV::os.group_add(name, pswd);
					if (gid != -1) {
						cout << "группа " << name << " с gid " << gid << " успешно добавлен." << endl;
					}
					else {
						cout << "во время добавления группы возникла ошибка." << endl
							<< "проверьте имя еще раз!" << endl;
					}
				}
				else {
					cout << "группу default создать нельзя";
				}
			}
			else if (args[0] == "get") {
				cout << "список групп: " << endl;
				for (auto& [gid, oid, name] : GV::os.group_get()) {
					printf("gid: %5d\toid: %5d\tname: %s\n", gid, oid, name.c_str());
				}
			}
			else if (args[0] == "delete") {
				string name, pswd;
				if (args.size() > 1) {
					name = args[1];
				}

				else {
					cout << "имя группы: ";
					std::getline(cin, name);
				}
				if (name != "default") {
					cout << "пароль: ";
					pswd = util::read_pswd();

					int uid = GV::os.group_del(name, pswd);
					if (uid != -1) {
						cout << "группа " << name << " успешно удалена!" << endl;
					}
					else {
						cout << "проверьте имя группы или пароль" << endl;
					}
				}
				else {
					cout << "группу default удалять нельзя";
				}
			}
			else if (args[0] == "rename") {
				//test 
			}
			else if (args[0] == "join") {

			}
		}
		else {
			cout << "add - создать группу" << endl;
			cout << "delete - удалить группу" << endl;
			cout << "rename - переименовать группу" << endl;
			cout << "join - присоединиться к группе" << endl;
		}
	};
}