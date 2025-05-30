#include <Windows.h>
#include <iostream>

using namespace std;


string HandleMailslotError(string functionName, int errorCode) {
    string msgText = functionName;

    switch (errorCode) {
    case ERROR_SEM_TIMEOUT:
        msgText += "The timeout period expired (ERROR_SEM_TIMEOUT).\n";
        break;
    case ERROR_ACCESS_DENIED:
        msgText += "Access denied (ERROR_ACCESS_DENIED).\n";
        break;
    case ERROR_INVALID_PARAMETER:
        msgText += "Invalid parameter (ERROR_INVALID_PARAMETER).\n";
        break;
    case ERROR_ALREADY_EXISTS:
        msgText += "Mailslot already exists (ERROR_ALREADY_EXISTS).\n";
        break;
    case ERROR_NOT_ENOUGH_MEMORY:
        msgText += "Not enough memory (ERROR_NOT_ENOUGH_MEMORY).\n";
        break;
    case ERROR_HANDLE_EOF:
        msgText += "End of file (ERROR_HANDLE_EOF).\n";
        break;
    case ERROR_FILE_NOT_FOUND:
        msgText += "Mailslot not found (ERROR_FILE_NOT_FOUND).\n";
        break;
    case ERROR_NO_SYSTEM_RESOURCES:
        msgText += "Not enough system resources (ERROR_NO_SYSTEM_RESOURCES).\n";
        break;
    case ERROR_IO_PENDING:
        msgText += "I/O operation is pending (ERROR_IO_PENDING).\n";
        break;
    case ERROR_BROKEN_PIPE:
        msgText += "Broken pipe (ERROR_BROKEN_PIPE).\n";
        break;
    case ERROR_INVALID_HANDLE:
        msgText += "Invalid handle (ERROR_INVALID_HANDLE).\n";
        break;
    case ERROR_INSUFFICIENT_BUFFER:
        msgText += "Insufficient buffer size (ERROR_INSUFFICIENT_BUFFER).\n";
        break;
    default:
        msgText += "Unknown error code";
        break;
    }

    return msgText;
}

int main() {

    HANDLE hM;
    DWORD wb;
    char wbuf[100] = "Hello server";
    char stopWord[100] = "STOP";

    //char host[100] = "\\\\.\\mailslot\\Box";
    char host[100] = "\\\\DESKTOP-NEGFC4R\\mailslot\\Box";
    //char host[100] = "\\\\MARTA\\mailslot\\Box";

    try {
        if ((hM = CreateFileA(host, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL)) == INVALID_HANDLE_VALUE) {
            throw HandleMailslotError("CreateFile: ", GetLastError());
        }
        
        //if (!WriteFile(hM, wbuf,
        //    sizeof(wbuf),
        //    //300,
        //    &wb, NULL)) {
        //    throw HandleMailslotError("WriteFile: ", GetLastError());
        //}

        //cout << "Message sended to server" << endl;

        for (int i = 0; i < 1000; i++) {
            if (!WriteFile(hM, wbuf,
                sizeof(wbuf),
                //300,
                &wb, NULL)) {
                throw HandleMailslotError("WriteFile: ", GetLastError());
            }
        }

        cout << "1000 messages sended to server" << endl;

        if (!WriteFile(hM, stopWord,
            sizeof(stopWord),
            //300,
            &wb, NULL)) {
            throw HandleMailslotError("WriteFile: ", GetLastError());
        }

        cout << "STOP message sended to server" << endl;

        CloseHandle(hM);
    }
    catch (string errorMsg) {
        cout << errorMsg << endl;
    }


    return 0;
}