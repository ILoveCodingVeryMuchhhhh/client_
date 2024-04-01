#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <thread>
#include <conio.h>

bool exit_ = false;
char msg[1024];

int main() {
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode;
	GetConsoleMode(hStdin, &mode);
	mode &= ~ENABLE_QUICK_EDIT_MODE;
	SetConsoleMode(hStdin, mode);

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		std::cout << "Error! code: 1";
		_getch();
		return -1;
	}

	SOCKET client;
	sockaddr_in saddr;

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(6140);
	std::string SIp;
	std::cin >> SIp;
	saddr.sin_addr.S_un.S_addr = inet_addr(SIp.c_str());

	client = socket(AF_INET, SOCK_STREAM, 0);

	if (client == SOCKET_ERROR) {
		std::cout << "Error code: 3\n";
		std::cout << WSAGetLastError();
		_getch();
		return -1;
	}

	int ReqNum = 0;
	for (ReqNum;
		connect(client, (sockaddr*)&saddr, sizeof(saddr)) == SOCKET_ERROR && ReqNum < 15;
		ReqNum++);
	if (ReqNum >= 15) {
		std::cout << "Error! code: 2";
		_getch();
		return -1;
	}

	std::thread r([=] {
		while (!exit_) {
			char chatmsg[1024];
			recv(client, chatmsg, 1024, 0);
			if (strcmp(chatmsg, "\\se") == 0) {
				std::cout << "服务端已关闭，正在退出...\n";
				exit_ = true;
				break;
			}
			std::cout << chatmsg << "\n";
		}
		}),
		s([&] {
		while (!exit_) {
			char msg[1024] = "\\O..";
			send(client, msg, sizeof(msg), 0);
			Sleep(1000);
		}
			});
	r.detach();
	s.detach();

	while (!exit_) {
		std::cin.getline(msg, 1024);
		if (strcmp(msg, "\\e") == 0) {
			std::cout << "正在退出...";
			exit_ = true;
			break;
		}
		send(client, msg, sizeof(msg), 0);
	}
	Sleep(1000);

	closesocket(client);
	WSACleanup();
	return 0;
}
