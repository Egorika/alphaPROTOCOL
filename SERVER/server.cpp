#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <iostream>
#include <winsock2.h>
#include <fstream>
#include <regex>
#pragma comment(lib, "WS2_32.lib")

using namespace std;

void send_email(char* msg) {
	regex regex("(\\w+\\.?\\w+@\\w+\\.\\w+) (.+)");
	cmatch match;
	string string, email;

	if (regex_match(msg, match, regex) && match.size() > 1) {
		email = match.str(1);
		string = match.str(2);
	}

	char * text = new char[string.size() + 1];
	copy(string.begin(), string.end(), text);
	text[string.size()] = '\0';
	
	char * mail = new char[email.size() + 1];
	copy(email.begin(), email.end(), mail);
	mail[email.size()] = '\0';

	strcat(mail, ".txt");
	strcat(text, " [false]\n");

	ofstream file;
	file.open(mail, ios::app);
	file << text;

	file.close();
}

char* read_email(char* msg) {
	regex regex("\\[false\\]");
	smatch match;

	strcat(msg, ".txt");

	ifstream file(msg);

	string temp[200];
	for(int i = 0; file; i++) {
		getline(file, temp[i]);
		temp[i]= regex_replace(temp[i], regex, "[true]\n");
	}
	file.close();

	fstream clear(msg, ios::out);
	clear.close();
	char* mail = new char[1000];
	ofstream file_new;
	file_new.open(msg, ios::app);
	for (int i = 0; i < temp->length(); i++) {
		file_new << temp[i];

		char * text = new char[temp[i].size() + 1];
		copy(temp[i].begin(), temp[i].end(), text);
		text[temp[i].size()] = '\n';

		strcat(mail, text);
	}
	mail[strlen (mail) + 1] = '\0';
	
	file_new.close();

	return mail;
	



}

DWORD WINAPI ThreadFunc(LPVOID client_socket)
{
	SOCKET sock = ((SOCKET *)client_socket)[0];
	regex regex_send("(\\w+\\.?\\w+@\\w+\\.\\w+) (.+)");
	regex regex_read("\\w+\\.?\\w+@\\w+\\.\\w+");
	smatch match;
	char msg[63] = "Отправить:\tвведите почту и сообщение\nПрочитать:\tвведите почту\n";
	char respond[200];
	send(sock, msg, sizeof(msg), 0);

	while (true)
	{
		recv(sock, respond, sizeof(respond), 0);

		string temp(respond);
		
		if (regex_search(temp, match, regex_send)) {
			send_email(respond);
			char success[50] = "Отправлено\n";
			send(sock, success, sizeof(success), 0);
		}
		else if (regex_search(temp, match, regex_read)) {
			
			char* mail = read_email(respond);
			send(sock, mail, sizeof(mail), 0);
		}
		else {
			char failure[50] = "Некорректный ввод\n";
			send(sock, failure, sizeof(failure), 0);
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
	// извлекаем сообщение из очереди
	SOCKET client_socket;    // сокет для клиента
	sockaddr_in client_addr; // адрес клиента (заполняется системой)
	int client_addr_size = sizeof(client_addr);
	// цикл извлечения запросов на подключение из очереди
	while ((client_socket = accept(s, (sockaddr *)&client_addr, &client_addr_size)))
	{
		// Вызов нового потока для обслуживания клиента
		DWORD thID;// thID идентификатор типа DWORD
		CreateThread(NULL, NULL, ThreadFunc, &client_socket, NULL, &thID);
	}
}