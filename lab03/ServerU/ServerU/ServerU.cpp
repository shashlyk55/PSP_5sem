#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "WinSock2.h"
#pragma comment (lib, "WS2_32.lib")

#include "Error.h"
#include <tchar.h>
#include <iostream>

using namespace std;

int main(int aegc, _TCHAR* argv[]) {
	SOCKET sS;
	WSADATA wsaData;

	try {
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
			throw SetErrorMsgText("Startup: ", WSAGetLastError());
		}


		if ((sS = socket(
			AF_INET,            // формат адреса(IPv4)
			SOCK_DGRAM,        // тип сокета(SOCK_DGRAM сокет для сообщений, SOCK_STREAM потоковый сокет)
			NULL                // протокол транспортного уровня
		)) == INVALID_SOCKET)
			throw SetErrorMsgText("socket: ", WSAGetLastError());

		/*int recvBufferSize = 500;
		if (setsockopt(sS, SOL_SOCKET, SO_RCVBUF, (const char*)&recvBufferSize, sizeof(recvBufferSize)) == SOCKET_ERROR)
		{
			throw SetErrorMsgText("setsockopt: ", WSAGetLastError());
		}*/


		SOCKADDR_IN serv;
		serv.sin_family = AF_INET;
		serv.sin_port = htons(2000);
		serv.sin_addr.s_addr = INADDR_ANY;

		if (bind(sS, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR) {
			throw SetErrorMsgText("bind: ", WSAGetLastError());
		}

		SOCKADDR_IN from;
		int lfrom;
		
		char bfrom[50];
		int lbfrom = 0;

		char* clientIP;
		int clientPort;

		SOCKADDR_IN to;

		char bto[50] = "The server has recieved your message!";
		int lobuf = 0;


		while (true) {

			memset(&from, 0, sizeof(from));
			lfrom = sizeof(from);

			lbfrom = 0;

			cout << "Waiting for message..." << endl << endl;
			

			if ((lbfrom = recvfrom(sS, bfrom, sizeof(bfrom), NULL, (sockaddr*)&from, &lfrom)) < 0) {
				throw SetErrorMsgText("recvFrom: ", WSAGetLastError());
			}


			clock_t start_time = clock();

			clientIP = inet_ntoa(from.sin_addr);
			clientPort = ntohs(from.sin_port);

			cout << "Getting from " << clientIP << ":" << clientPort << endl;
			cout << "Message from client: " << bfrom << endl << endl;

			to = from;
			lobuf = 0;

			strcpy(bto, bfrom);

			if ((lobuf = sendto(sS, bto, strlen(bto) + 1, NULL, (sockaddr*)&to, sizeof(to))) == SOCKET_ERROR) {
				throw SetErrorMsgText("sendTo: ", WSAGetLastError());
			}

			cout << "Message sended to client!" << endl << endl;

			clock_t end_time = clock();

			cout << "Processing time: " << (double)(end_time - start_time) / CLOCKS_PER_SEC << endl << endl;

		}


		if (closesocket(sS) == SOCKET_ERROR)
			throw SetErrorMsgText("closesocket: ", WSAGetLastError());

		if (WSACleanup() == SOCKET_ERROR)
			throw SetErrorMsgText("Cleanup: ", WSAGetLastError());


	}
	catch (string errorMsgText) {
		cout << endl << errorMsgText << endl;
	}


	return 0;
}