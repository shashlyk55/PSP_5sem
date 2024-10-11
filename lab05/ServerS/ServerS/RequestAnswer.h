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

int GetServersQuantity(
	char* call,				// [in] �������� �������
	short port				// [in] ����� ����� ������
);
