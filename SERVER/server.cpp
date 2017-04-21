#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <iostream>
#include <winsock2.h>
#include <fstream>
#include <regex>
#pragma comment(lib, "WS2_32.lib")

using namespace std;

void send_email(std::string msg) {
	regex regex("(\\w+\\.?\\w+@\\w+\\.\\w+) (.+)");
	smatch match;
	string string, email;
	if (regex_match(msg, match, regex)) {
		email = match.str(1);
		string = match.str(2);
	}
	cout << "Текущая почта\n\t[" << email << "]"<<endl;
	email.append(".txt");
	string.append("\n");
	ofstream file;
	file.open(email, ios::app);
	file << string;
	cout << "Файл\n\t[" << email << "]\nизменен\\создан" << endl;
	file.close();
}

string read_email(std::string msg) {
	string str = "Почта [" + msg + "]:\n";
	msg.append(".txt");
	ifstream file(msg);
	if (!file) cout << "Файл не существует" << endl;
	for(int i = 0; file; i++){
		string temp;
		getline(file, temp);
		str.append("\t[" + to_string(i) + "]: " + temp + "\n");
	}
	cout << "Все письма из файла\n\t[" << msg << "]\nпрочитаны" << endl;
	str.append("Конец\n");
	file.close();

	return str;
}

DWORD WINAPI ThreadFunc(LPVOID client_socket)
{
	SOCKET sock = ((SOCKET *)client_socket)[0];
	regex regex_send("(\\w+\\.?\\w+@\\w+\\.\\w+) (.+)");
	regex regex_read("\\w+\\.?\\w+@\\w+\\.\\w+");
	smatch match;
	string msg = "Отправить:\t[введите почту и сообщение]\nПрочитать:\t[введите почту]\n";
	char buffer[4096];
	send(sock, msg.data(), msg.size(), 0);

	while (true)
	{

		int check = recv(sock, buffer, sizeof(buffer), 0);
		if (check <= 0) break;
		buffer[check] = '\0';
		string temp(buffer);
		if (regex_search(temp, match, regex_send)) {
			send_email(temp);
			string success = "[Отправлено]\n";
			send(sock, success.data(), success.length(), 0);
		}
		else if (regex_search(temp, match, regex_read)) {
			string text = read_email(temp);
			send(sock, text.data(), text.length(), 0);

		}
		else {
			string failure = "[Некорректный ввод]\n";
			send(sock, failure.data(), failure.length(), 0);
		}
	}
	closesocket(sock);
	return 0;
}

void main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) { return; }

	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(1280);
	local_addr.sin_addr.s_addr = 0;
	bind(s, (sockaddr *)&local_addr, sizeof(local_addr));
	int c = listen(s, 5);
	cout << "Сервер готов" << endl;
	SOCKET client_socket;   
	sockaddr_in client_addr; 
	int client_addr_size = sizeof(client_addr);
	while ((client_socket = accept(s, (sockaddr *)&client_addr, &client_addr_size)))
	{

		DWORD thID;
		CreateThread(NULL, NULL, ThreadFunc, &client_socket, NULL, &thID);
	}
}