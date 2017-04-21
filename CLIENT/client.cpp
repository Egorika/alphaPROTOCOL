#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <iostream>
#include <string>
#pragma comment(lib, "WS2_32.lib")

using namespace std;

int main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) return -1;
	struct sockaddr_in peer;
	peer.sin_family = AF_INET;
	peer.sin_port = htons(1280);
	peer.sin_addr.s_addr = inet_addr("127.0.0.1");
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	connect(sock, (struct sockaddr*) &peer, sizeof(peer));
	char respond[200];
	string reply;
	int check = 0; 
	while (true) {
		check = recv(sock, respond, sizeof(respond), 0);
		if (check <= 0) break;
		respond[check] = '\0';
		cout << respond;
		respond[0] = '\0';
		getline(cin, reply);
		if (reply == "exit") exit(EXIT_SUCCESS);
		send(sock, reply.data(), reply.length(), 0);
	}

	WSACleanup();
	return 0;
}
