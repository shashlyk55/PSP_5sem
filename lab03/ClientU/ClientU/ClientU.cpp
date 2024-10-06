#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <cstdio>
#include <thread>
#include <chrono>
#include <iostream>
#include <tchar.h>
#include "Error.h"

using namespace std;

int extract_and_increment(const std::string& message) {
	size_t pos = message.find_last_of(' ');
	if (pos != std::string::npos) {
		std::string num_str = message.substr(pos + 1);
		int num = stoi(num_str);
		return num + 1;
	}
	return 1;
}

int main(int argc, _TCHAR* argv[])
{
	SOCKET cS;              // дескриптор сокета, клиентский сокет
	WSADATA wsaData;

	try {
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw SetErrorMsgText("Startup: ", WSAGetLastError());

		int quantity = 0;
		cout << "Message quantity: " << endl;
		cin >> quantity;


		if ((cS = socket(
			AF_INET,
			SOCK_DGRAM,
			NULL
		)) == INVALID_SOCKET)
			throw SetErrorMsgText("socket: ", WSAGetLastError());

		SOCKADDR_IN to;
		to.sin_family = AF_INET;
		to.sin_port = htons(2000);
		to.sin_addr.s_addr = inet_addr("127.0.0.1");

		char bto[50] = "Hello server";
		int lbto = 0;

		for (int i = 0; i < quantity; i++) {

			snprintf(bto, sizeof(bto), "Hello server %d", i + 1);

			if ((lbto = sendto(cS, "ServerB", strlen(bto) + 1, NULL, (sockaddr*)&to, sizeof(to)) == SOCKET_ERROR))
				throw SetErrorMsgText("sendto: ", WSAGetLastError());

			cout << "Message sended to server!" << endl << endl;


			SOCKADDR_IN from;
			memset(&from, 0, sizeof(from));
			int lfrom = sizeof(from);

			char bfrom[50];
			int lbfrom = 0;

			cout << "Waiting for message..." << endl << endl;

			if ((lbfrom = recvfrom(cS, bfrom, sizeof(bfrom), NULL, (sockaddr*)&from, &lfrom)) < 0)
				throw SetErrorMsgText("recvfrom: ", WSAGetLastError());

			cout << "Message from server: " << bfrom << endl << endl;


		}

		if (closesocket(cS) == SOCKET_ERROR)
			throw SetErrorMsgText("closesocket: ", WSAGetLastError());

		if (WSACleanup() == SOCKET_ERROR)
			throw SetErrorMsgText("Cleanup: ", WSAGetLastError());

		
	}
	catch (string errorMessage) {
		cout << endl << errorMessage;
	}

	return 0;
}


