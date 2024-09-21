#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "WinSock2.h"                   // заголовок  WS2_32.dll
#pragma comment (lib, "WS2_32.lib")     // экспорт  WS2_32.dll

#include <iostream>
#include <tchar.h>
#include "Error.h"


using namespace std;



int main(int argc, _TCHAR* argv[])
{
    SOCKET sS;              // дескриптор сокета, серверный сокет
    WSADATA wsaData;

    try {
        if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
            throw SetErrorMsgText("Startup: ", WSAGetLastError());

        if ((sS = socket(
            AF_INET,            // формат адреса(IPv4)
            SOCK_STREAM,        // тип сокета(SOCK_DGRAM сокет для сообщений, SOCK_STREAM потоковый сокет)
            NULL                // протокол транспортного уровня
        )) == INVALID_SOCKET)
            throw SetErrorMsgText("socket: ", WSAGetLastError());


        SOCKADDR_IN serv;                            // параметры сокета sS
        
        serv.sin_family = AF_INET;                  // используется IP-адресация
        serv.sin_port = htons(2000);                // порт 2000
        serv.sin_addr.s_addr = INADDR_ANY;     // любой собственный IP-адрес

        if (bind(sS, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR)
            throw SetErrorMsgText("bind: ", WSAGetLastError());

        if (listen(sS, SOMAXCONN) == SOCKET_ERROR)
            throw SetErrorMsgText("listen: ", WSAGetLastError());


        SOCKET cS;                          // сокет обмена данными с клиентом
        SOCKADDR_IN clnt;                   // параметры сокета клиента
        memset(&clnt, 0, sizeof(clnt));     // обнулить память
        int lclnt = sizeof(clnt);           // размер SOCKADDR_IN

        char ibuf[50],                      // буфер ввода
            obuf[50] = "server: принято ";  // буфер вывода
        int libuf = 0,                      // количество принятых байт
            lobuf = 0;                      // количество отправленных байт
        while (true) {
            cout << "Waiting for client connect..." << endl;

            if ((cS = accept(sS, (sockaddr*)&clnt, &lclnt)) == INVALID_SOCKET)
                throw SetErrorMsgText("accept: ", WSAGetLastError());

            char* clientIP = inet_ntoa(clnt.sin_addr);
            int clientPort = ntohs(clnt.sin_port);

            cout << "Connected to client " << clientIP << ":" << clientPort << endl;

            clock_t start_time = clock();

            while ((libuf = recv(cS, ibuf, sizeof(ibuf), NULL)) >= 0) {
                if (libuf == SOCKET_ERROR) {
                    throw SetErrorMsgText("recv: ", WSAGetLastError());
                }

                if (libuf == 0) {
                    cout << "Client sent a message of zero length" << endl;
                    break;
                }

                cout << "Message recieved 1: " << ibuf << endl;

                if ((lobuf = send(cS, ibuf, strlen(ibuf) + 1, NULL)) == SOCKET_ERROR)
                    throw SetErrorMsgText("send: ", WSAGetLastError());

                libuf = recv(cS, ibuf, sizeof(ibuf), NULL);

                if (libuf == SOCKET_ERROR) {
                    throw SetErrorMsgText("recv: ", WSAGetLastError());
                }

                if (libuf == 0) {
                    cout << "Client sent a message of zero length" << endl;
                    break;
                }

                cout << "Message recieved 2: " << ibuf << endl;

                Sleep(5);
            }

            clock_t end_time = clock();

            double elapsedTime = double(end_time - start_time) / CLOCKS_PER_SEC;

            cout << "Messaging is taking " << elapsedTime << " seconds" << endl;

            if (closesocket(cS) == SOCKET_ERROR)
                throw SetErrorMsgText("closesocket: ", WSAGetLastError());

            cout << "Client disconnected" << endl;
        }

        if (closesocket(sS) == SOCKET_ERROR)
            throw SetErrorMsgText("closesocket: ", WSAGetLastError());

        if (WSACleanup() == SOCKET_ERROR)
            throw SetErrorMsgText("Cleanup: ", WSAGetLastError());
        
    }
    catch (string errorMsgText) {
        cout << endl << errorMsgText;
    }
    

    return 0;
}