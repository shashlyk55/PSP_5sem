#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "WinSock2.h"
#pragma comment (lib, "WS2_32.lib")

#include "Error.h"
#include <tchar.h>
#include <iostream>

using namespace std;

bool GetRequestFromClient(
	char* name,				// [in] �������� �������
	short port,				// [in] ����� ��������������� �����
	struct sockaddr* from,	// [out] ��������� �� SOCKADDR_IN
	int* flen				// [out] ��������� �� ����� from
);

bool PutAnswerToClient(
	char* name,			// [in] �������� �������
	struct sockaddr* to,	// [in] ��������� �� SOCKADDR
	int* lto			// [in] ��������� �� ����� from
);

int CountServers(
	char* call,				// [in] �������� �������
	short port				// [in] ����� ����� ������
);


int main(int aegc, _TCHAR* argv[]) {

	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	try {
		WSADATA wsaData;

		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
			throw SetErrorMsgText("Startup: ", WSAGetLastError());
		}

		char serverName[] = "ServerS";
		short port = 2000;
		char computerName[] = "DESKTOP-9MC22NJ";

		//cout << serverName << "servers count: " << CountServers(serverName, port) << endl;

		cout << "Server computer name: " << computerName << endl;

		while (true) {
			SOCKADDR_IN clientAddr;
			int clientAddrLen;

			if (GetRequestFromClient(serverName, port, (struct sockaddr*)&clientAddr, &clientAddrLen))
			{
				char* clientIp = inet_ntoa(clientAddr.sin_addr);
				int clientIpLen = sizeof(clientIp);
				struct hostent* he;
				if ((he = gethostbyaddr((char*)&clientAddr.sin_addr, sizeof(clientAddr.sin_addr), AF_INET)) == NULL) {
						throw SetErrorMsgText("hostname: ", WSAHOST_NOT_FOUND);
				}
				cout << "Client socket: " << clientIp << ":" << port << endl;
				cout << " Client computer name: " << he->h_name << endl;

				PutAnswerToClient(serverName, (struct sockaddr*)&clientAddr, &clientAddrLen);
			}
		}

		if (WSACleanup() == SOCKET_ERROR)
			throw SetErrorMsgText("Cleanup: ", WSAGetLastError());
	}
	catch (string errorMsgText) {
		cout << endl << errorMsgText << endl;
	}


	return 0;
}




bool GetRequestFromClient(
	char* name,				// [in] �������� �������
	short port,				// [in] ����� ��������������� �����
	struct sockaddr* from,	// [out] ��������� �� SOCKADDR_IN
	int* lfrom				// [out] ��������� �� ����� from
) {
	SOCKET sS;

	if ((sS = socket(
		AF_INET,            // ������ ������(IPv4)
		SOCK_DGRAM,        // ��� ������(SOCK_DGRAM ����� ��� ���������, SOCK_STREAM ��������� �����)
		NULL                // �������� ������������� ������
	)) == INVALID_SOCKET)
		throw SetErrorMsgText("socket: ", WSAGetLastError());

	int timeout = 5000;
	if (setsockopt(sS, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) == SOCKET_ERROR) {
		throw SetErrorMsgText("setsockopt: ", WSAGetLastError());
	}

	SOCKADDR_IN serv;
	serv.sin_family = AF_INET;
	serv.sin_port = htons(port);
	//serv.sin_addr.s_addr = inet_addr("127.0.0.2");
	serv.sin_addr.s_addr = INADDR_ANY;

	if (bind(sS, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR) {
		throw SetErrorMsgText("bind: ", WSAGetLastError());
	}

	cout << "Server started on: " << inet_ntoa(serv.sin_addr) << ":" << ntohs(serv.sin_port) << endl;

	char bfrom[50];
	int lbfrom;

	char* clientIp;
	int clientPort;
	SOCKADDR_IN* client;

	while (true) {

		memset(&bfrom, 0, sizeof(bfrom));
		*lfrom = sizeof(struct sockaddr_in);

		cout << "Waiting for message..." << endl << endl;

		if ((lbfrom = recvfrom(sS, bfrom, sizeof(bfrom), NULL, (sockaddr*)from, lfrom)) < 0) {
			int errCode = WSAGetLastError();

			if (errCode == WSAETIMEDOUT) {
				cout << "recvFrom: waiting time is expired" << endl;
				if (closesocket(sS) == SOCKET_ERROR)
					throw SetErrorMsgText("closesocket: ", WSAGetLastError());
				return false;
			}
			else {
				if (closesocket(sS) == SOCKET_ERROR)
					throw SetErrorMsgText("closesocket: ", WSAGetLastError());
				throw SetErrorMsgText("recvFrom: ", WSAGetLastError());
			}
		}

		if (strcmp(bfrom, name) == 0) {
			client = (SOCKADDR_IN*)from;
			clientIp = inet_ntoa(client->sin_addr);
			clientPort = ntohs(client->sin_port);

			cout << "Recieved correct server name from client"
				<< "(" << clientIp << ":" << clientPort << "): " << bfrom << endl;

			if (closesocket(sS) == SOCKET_ERROR)
				throw SetErrorMsgText("closesocket: ", WSAGetLastError());
			return true;
		}
	}

	if (closesocket(sS) == SOCKET_ERROR)
		throw SetErrorMsgText("closesocket: ", WSAGetLastError());

	return false;
}


bool PutAnswerToClient(
	char* name,			// [in] �������� �������
	struct sockaddr* to,	// [in] ��������� �� SOCKADDR
	int* lto			// [in] ��������� �� ����� from
) {
	SOCKET sS;

	if ((sS = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET) {
		throw SetErrorMsgText("socket: ", WSAGetLastError());
	}

	if (sendto(sS, name, strlen(name) + 1, NULL, (sockaddr*)to, *lto) == SOCKET_ERROR) {
		throw SetErrorMsgText("sendto: ", WSAGetLastError());
	}

	cout << "Connection request sended" << endl << endl;

	if (closesocket(sS) == SOCKET_ERROR) {
		throw SetErrorMsgText("closesocket: ", WSAGetLastError());
	}

	return true;
}


int CountServers(
	char* call,				// [in] �������� �������
	short port				// [in] ����� ����� ������				
) {

	SOCKET sS;

	if ((sS = socket(
		AF_INET,
		SOCK_DGRAM,
		NULL
	)) == INVALID_SOCKET)
		throw SetErrorMsgText("socket: ", WSAGetLastError());

	int timeout = 10000;
	if (setsockopt(sS, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) == SOCKET_ERROR) {
		throw SetErrorMsgText("opt: ", WSAGetLastError());
	}

	int optval = 1;
	if (setsockopt(sS, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(int)) == SOCKET_ERROR) {
		throw SetErrorMsgText("opt: ", WSAGetLastError());
	}

	SOCKADDR_IN all;
	all.sin_family = AF_INET;
	all.sin_port = htons(port);
	all.sin_addr.s_addr = inet_addr("127.255.255.255");
	//all.sin_addr.s_addr = INADDR_BROADCAST;

	int lbto = 0;

	if ((lbto = sendto(sS, call, strlen(call) + 1, NULL, (sockaddr*)&all, sizeof(all)) == SOCKET_ERROR))
		throw SetErrorMsgText("sendto: ", WSAGetLastError());

	char bfrom[50];
	memset(&bfrom, 0, sizeof(bfrom));
	int lbfrom = 0;

	int serversQuantity = 0;

	SOCKADDR_IN serv;
	memset(&serv, 0, sizeof(serv));
	int servLen = sizeof(serv);

	while (true) {
		memset(&serv, 0, sizeof(serv));
		memset(&bfrom, 0, sizeof(bfrom));

		if (recvfrom(sS, bfrom, sizeof(bfrom), NULL, (sockaddr*)&serv, &servLen) == SOCKET_ERROR) {
			if (WSAGetLastError() == WSAETIMEDOUT) {
				break;
			}
			else throw SetErrorMsgText("recv: ", WSAGetLastError());
		}
		if (strcmp(bfrom, call) == 0) {
			serversQuantity++;
		}
	}

	if (closesocket(sS) == SOCKET_ERROR)
		throw SetErrorMsgText("closesocket: ", WSAGetLastError());

	return serversQuantity;
}

