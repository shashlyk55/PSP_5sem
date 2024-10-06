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

bool GetServer(
	char* call,				// [in] �������� �������
	short port,				// [in] ����� ����� ������
	struct sockaddr* from,	// [out] ��������� �� SOCKADDR_IN
	int* flen				// [out] ��������� �� ������ from
);

int main(int argc, _TCHAR* argv[])
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	WSADATA wsaData;

	try {
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw SetErrorMsgText("Startup: ", WSAGetLastError());

		char serverName[] = "ServerB";
		int port = 2000;

		SOCKADDR_IN servAddr;
		memset(&servAddr, 0, sizeof(servAddr));
		int servAddrLen = sizeof(servAddr);

		if (GetServer(serverName, port, (sockaddr*)&servAddr, &servAddrLen)) {
			cout << "Server found " << endl;
			cout << inet_ntoa(servAddr.sin_addr) << ":" << htons(servAddr.sin_port) << endl;
		}

		if (WSACleanup() == SOCKET_ERROR)
			throw SetErrorMsgText("Cleanup: ", WSAGetLastError());
	}
	catch (string errorMessage) {
		cout << endl << errorMessage;
	}

	return 0;
}



bool GetServer(
	char* call,				// [in] �������� �������
	short port,				// [in] ����� ����� ������
	struct sockaddr* from,	// [out] ��������� �� SOCKADDR_IN
	int* lfrom				// [out] ��������� �� ������ from
) {

	SOCKET cS;

	if ((cS = socket(
		AF_INET,
		SOCK_DGRAM,
		NULL
	)) == INVALID_SOCKET)
		throw SetErrorMsgText("socket: ", WSAGetLastError());

	int optval = 1;
	if (setsockopt(cS, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(int)) == SOCKET_ERROR) {
		throw SetErrorMsgText("setsockopt: ", WSAGetLastError());
	}

	int timeout = 3000;
	if (setsockopt(cS, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) == SOCKET_ERROR) {
		throw SetErrorMsgText("setsockopt: ", WSAGetLastError());
	}

	SOCKADDR_IN all;
	all.sin_family = AF_INET;
	all.sin_port = htons(port);
	all.sin_addr.s_addr = inet_addr("127.255.255.255");
	//all.sin_addr.s_addr = INADDR_BROADCAST;

	int lbto = 0;

	if ((lbto = sendto(cS, call, strlen(call) + 1, NULL, (sockaddr*)&all, sizeof(all)) == SOCKET_ERROR))
		throw SetErrorMsgText("sendto: ", WSAGetLastError());

	cout << "Messages sended to servers!" << endl << endl;

	char bfrom[50];
	int lbfrom;
	memset(&bfrom, 0, sizeof(bfrom));

	char* clientIp;
	int clientPort;
	SOCKADDR_IN* client;


	while (true)
	{
		int result = recvfrom(cS, bfrom, sizeof(bfrom), NULL, from, lfrom);
		if (result == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAETIMEDOUT) {
				if (closesocket(cS) == SOCKET_ERROR)
					throw SetErrorMsgText("closesocket: ", WSAGetLastError());

				return false;
			}
			else {
				if (closesocket(cS) == SOCKET_ERROR)
					throw SetErrorMsgText("closesocket: ", WSAGetLastError());

				throw SetErrorMsgText("recvfrom: ", WSAGetLastError());
			}
		}

		client = (SOCKADDR_IN*)from;
		clientIp = inet_ntoa(client->sin_addr);
		clientPort = ntohs(client->sin_port);

		if (strcmp(call, bfrom) == 0) {
			cout << "Received correct server name from server " << clientIp << ":" << clientPort << ": " << bfrom << endl;
		}
		else {
			cout << "Received uncorrect server name from server: " << bfrom << endl;
		}

	}
	if (closesocket(cS) == SOCKET_ERROR)
		throw SetErrorMsgText("closesocket: ", WSAGetLastError());

	return true;


	/*if ((lbfrom = recvfrom(cS, bfrom, sizeof(bfrom), NULL, from, lfrom)) == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAETIMEDOUT)
			return false;
		else throw SetErrorMsgText("recv: ", WSAGetLastError());
	}

	if (strcmp(bfrom, call) == 0) {
		if (closesocket(cS) == SOCKET_ERROR)
			throw SetErrorMsgText("closesocket: ", WSAGetLastError());
		return true;
	}
	else {
		memset(&from, 0, sizeof(from));
		return false;
	}*/


	/*while (true) {

		memset(&bfrom, 0, sizeof(bfrom));

		cout << "Waiting for server answer..." << endl << endl;

		if ((lbfrom = recvfrom(cS, bfrom, sizeof(bfrom), NULL, from, lfrom)) < 0) {
			if (GetLastError() == WSAETIMEDOUT) {
				if (closesocket(cS) == SOCKET_ERROR)
					throw SetErrorMsgText("closesocket: ", WSAGetLastError());
				return false;
			}
			if (closesocket(cS) == SOCKET_ERROR)
				throw SetErrorMsgText("closesocket: ", WSAGetLastError());
			throw SetErrorMsgText("recvfrom: ", WSAGetLastError());
		}

		client = (SOCKADDR_IN*)from;
		clientIp = inet_ntoa(client->sin_addr);
		clientPort = ntohs(client->sin_port);

		cout << "Message from server"
			<< "(" << clientIp << ":" << clientPort << "): " << bfrom << endl;

		if (strcmp(bfrom, call) == 0) {
			
			return true;
		}
	}

	if (closesocket(cS) == SOCKET_ERROR)
		throw SetErrorMsgText("closesocket: ", WSAGetLastError());

	return false;*/
}




