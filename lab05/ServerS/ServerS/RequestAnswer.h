#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "WinSock2.h"
#pragma comment (lib, "WS2_32.lib")

#include "Error.h"
#include <tchar.h>
#include <iostream>

using namespace std;

bool GetRequestFromClient(
	char* name,				// [in] позывной сервера
	short port,				// [in] номер прослушиваемого порта
	struct sockaddr* from,	// [out] указатель на SOCKADDR_IN
	int* flen				// [out] указатель на длину from
);

bool PutAnswerToClient(
	char* name,			// [in] позывной сервера
	struct sockaddr* to,	// [in] указатель на SOCKADDR
	int* lto			// [in] указатель на рзмер from
);

int GetServersQuantity(
	char* call,				// [in] позывной сервера
	short port				// [in] номер порта севера
);
