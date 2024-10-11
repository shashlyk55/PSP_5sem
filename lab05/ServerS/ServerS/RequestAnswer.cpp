#include "RequestAnswer.h"


bool GetRequestFromClient(
	char* name,				// [in] позывной сервера
	short port,				// [in] номер прослушиваемого порта
	struct sockaddr* from,	// [out] указатель на SOCKADDR_IN
	int* lfrom				// [out] указатель на длину from
) {
	SOCKET sS;

	if ((sS = socket(
		AF_INET,            // формат адреса(IPv4)
		SOCK_DGRAM,        // тип сокета(SOCK_DGRAM сокет для сообщений, SOCK_STREAM потоковый сокет)
		NULL                // протокол транспортного уровня
	)) == INVALID_SOCKET)
		throw SetErrorMsgText("socket: ", WSAGetLastError());

	SOCKADDR_IN serv;
	serv.sin_family = AF_INET;
	serv.sin_port = htons(port);
	serv.sin_addr.s_addr = INADDR_ANY;

	if (bind(sS, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR) {
		throw SetErrorMsgText("bind: ", WSAGetLastError());
	}

	char bfrom[50];
	int lbfrom = 0;

	char bto[50] = "The server has recieved your message!";
	int lobuf = 0;

	while (true) {

		memset(bfrom, 0, sizeof(bfrom));
		*lfrom = sizeof(struct sockaddr_in);

		cout << "Waiting for message..." << endl << endl;

		if ((lbfrom = recvfrom(sS, bfrom, sizeof(bfrom), NULL, (sockaddr*)from, lfrom)) < 0) {
			int errCode = WSAGetLastError();

			if (errCode == WSAETIMEDOUT) {
				cout << "recvFrom: " << WSAGetLastError() << endl;
				if (closesocket(sS) == SOCKET_ERROR)
					throw SetErrorMsgText("closesocket: ", WSAGetLastError());
				return false;
			}
			if (closesocket(sS) == SOCKET_ERROR)
				throw SetErrorMsgText("closesocket: ", WSAGetLastError());
			throw SetErrorMsgText("recvFrom: ", WSAGetLastError());
		}

		cout << "Message from client: " << bfrom << endl;

		if (!strncmp(bfrom, name, strlen(name))) {
			cout << "Recieved correct server name" << endl;
			if (closesocket(sS) == SOCKET_ERROR)
				throw SetErrorMsgText("closesocket: ", WSAGetLastError());
			return true;
		}

	}

	if (closesocket(sS) == SOCKET_ERROR)
		throw SetErrorMsgText("closesocket: ", WSAGetLastError());

}


bool PutAnswerToClient(
	char* name,			// [in] позывной сервера
	struct sockaddr* to,	// [in] указатель на SOCKADDR
	int* lto			// [in] указатель на рзмер from
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


int GetServersQuantity(
	char* call,				// [in] позывной сервера
	short port				// [in] номер порта севера				// [out] указатель на размер from
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
		throw SetErrorMsgText("opt: ", WSAGetLastError());
	}

	SOCKADDR_IN all;
	all.sin_family = AF_INET;
	all.sin_port = htons(port);
	all.sin_addr.s_addr = INADDR_BROADCAST;

	int lbto = 0;

	if ((lbto = sendto(cS, call, strlen(call) + 1, NULL, (sockaddr*)&all, sizeof(all)) == SOCKET_ERROR))
		throw SetErrorMsgText("sendto: ", WSAGetLastError());

	cout << "Messages sended!" << endl << endl;

	char bfrom[50];
	memset(bfrom, 0, sizeof(bfrom));
	int lbfrom = 0;

	int serversQuantity = 0;

	cout << "Waiting for answers..." << endl << endl;

	SOCKADDR_IN from;
	memset(&from, 0, sizeof(from));
	int lfrom = sizeof(lfrom);

	while (lbfrom = recvfrom(cS, bfrom, sizeof(bfrom), NULL, (sockaddr*)&from, &lfrom)) {
		if (lbfrom < 0 ) {
			if (GetLastError() == WSAETIMEDOUT) {
				if (closesocket(cS) == SOCKET_ERROR)
					throw SetErrorMsgText("closesocket: ", WSAGetLastError());
				return serversQuantity;
			}
			if (closesocket(cS) == SOCKET_ERROR)
				throw SetErrorMsgText("closesocket: ", WSAGetLastError());
		}
		//throw SetErrorMsgText("recvfrom: ", WSAGetLastError());

		cout << "Message from server: " << bfrom << endl << endl;

		serversQuantity++;
	}

	if (closesocket(cS) == SOCKET_ERROR)
		throw SetErrorMsgText("closesocket: ", WSAGetLastError());

	return serversQuantity;
}

