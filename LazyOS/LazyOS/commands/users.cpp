#include "../globals.h"
#include <iostream>
#include "../utils.h"

using std::string;
using std::cout;
using std::cin;
using std::endl;

void set_users_commands() {
	GV::cmds["who"] = [](std::vector<string> args) {
		cout << "������ ��: " << GV::os.current_user.login << endl;
	};

	GV::cmds["users"] = [](std::vector<string> args) {
		if (!args.empty()) {
			if (args[0] == "add") {
				string login, pswd;
				if (args.size() > 1) {
					login = args[1];
				}
				else {
					cout << "�����: ";
					std::getline(cin, login);
				}
				cout << "������: ";
				pswd = util::read_pswd();

				GV::os.sudo();
				int uid = GV::os.user_add(login, pswd);
				GV::os.suend();
				if (uid != -1) {
					cout << "������������ " << login << " � uid " << uid << " ������� ��������." << endl;
				}
				else {
					cout << "�� ����� ���������� ������������ �������� ������." << endl
						<< "��������� ����� ��� ���!" << endl;
				}
			}
			else if (args[0] == "get") {
				cout << "������ �������������: " << endl;
				GV::os.sudo();
				for (auto& [uid, gid, login] : GV::os.user_get()) {
					printf("uid: %5d\tgid: %5d\tlogin: %s\n", uid, gid, login.c_str());
				}
				GV::os.suend();

			}
			else if (args[0] == "delete") {
				string login, pswd;
				if (args.size() > 1) {
					login = args[1];
				}
				else {
					cout << "�����: ";
					std::getline(cin, login);
				}
				cout << "������: ";
				pswd = util::read_pswd();

				GV::os.sudo();
				int uid = GV::os.user_login(login, pswd);
				GV::os.suend();
				if (uid != -1) {
					GV::os.sudo();
					GV::os.user_del(login);
					GV::os.suend();
					cout << "������������ " << login << " ������� �����!" << endl;
				}
				else {
					cout << "�� ������ ����� ��� ������" << endl;
				}
			}
			else if (args[0] == "rename") {
				string login, pswd, new_login;
				if (args.size() > 1) {
					login = args[1];
				}
				else {
					cout << "�����: ";
					std::getline(cin, login);
				}
				cout << "������: ";
				pswd = util::read_pswd();

				cout << "����� �����: ";
				std::getline(cin, new_login);

				GV::os.sudo();
				int uid = GV::os.user_login(login, pswd);
				GV::os.suend();
				if (uid != -1) {
					GV::os.sudo();
					GV::os.user_rnm(login, new_login);
					GV::os.suend();
					cout << "������������ " << login << " ������� ������������!" << endl;
				}
				else {
					cout << "�� ������ ����� ��� ������" << endl;
				}
			}
			else if (args[0] == "passwd") {
				string login, pswd, new_pswd;
				if (args.size() > 1) {
					login = args[1];
				}
				else {
					cout << "�����: ";
					std::getline(cin, login);
				}
				cout << "������: ";
				pswd = util::read_pswd();

				cout << "����� ������: ";
				new_pswd = util::read_pswd();

				GV::os.sudo();
				int uid = GV::os.user_login(login, pswd);
				GV::os.suend();
				if (uid != -1) {
					GV::os.sudo();
					GV::os.user_pswd(login, new_pswd);
					GV::os.suend();
					cout << "������������ " << login << " ������� ������ ������!" << endl;
				}
				else {
					cout << "�� ������ ����� ��� ������" << endl;
				}
			}
			else if (args[0] == "login") {
				string login, pswd;
				if (args.size() > 1) {
					login = args[1];
				}
				else {
					cout << "�����: ";
					std::getline(cin, login);
				}
				cout << "������: ";
				pswd = util::read_pswd();

				GV::os.sudo();
				int uid = GV::os.user_login(login, pswd);
				GV::os.suend();
				if (uid != -1) {
					GV::os.sudo();
					auto [_uid, _gid, _login] = GV::os.user_get()[uid];
					GV::os.suend();
					GV::os.current_user = LazyOS::user(uid, login, pswd);
					GV::os.current_user.gid = _gid;
				}
				else {
					cout << "�� ������ ����� ��� ������" << endl;
				}
			}
			else {
				cout << "����������� �������� " << args[0] << endl;
			}
		}
		else {
			//print legend
			cout << "get - �������� ������ �������������" << endl;
			cout << "add - �������� ������������" << endl;
			cout << "delete - ������� ������������" << endl;
			cout << "rename - ������������� ������������" << endl;
			cout << "passwd - ����� ������ ������������" << endl;
			cout << "login - ������� ������������" << endl;
		}
	};
}