#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


//--------
// C++ is behaving as client and send request to python server

#define DEFAULT_BUFLEN 512
#define CLIENT_PORT 12000
#define SERVER_PORT  12001

int nTimeout = 1000; //1 seconds

int main(int argc, char **argv) 
{
    WSADATA wsaData;
    struct addrinfo* result = NULL, * ptr = NULL, hints;

    ///// Initialize Winsock
    int iResult;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }
         
    // Create a socket for sending data
    // Also bind the port CLIENT_PORT
    SOCKET SendSocket = INVALID_SOCKET;
    sockaddr_in RecvAddr;

    SendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    setsockopt(SendSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&nTimeout, sizeof(int));

    if (SendSocket == INVALID_SOCKET) {
        wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("127.0.0.1");
    service.sin_port = htons(CLIENT_PORT);

    //----------------------
    // Bind the socket.
    iResult = bind(SendSocket, (SOCKADDR*)&service, sizeof(service));
    if (iResult == SOCKET_ERROR) {
        wprintf(L"bind failed with error %u\n", WSAGetLastError());
        closesocket(SendSocket);
        WSACleanup();
        return 1;
    }
    else
        wprintf(L"bind returned success\n");

    //---------------------------------
    // Define destinate address
    RecvAddr.sin_family = AF_INET;
    RecvAddr.sin_port = htons(SERVER_PORT);
    RecvAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    //---------------------------------------------
    // Send a datagram to the destinate address
    const char* sendbuf = "thang cho Liam";
    iResult = sendto(SendSocket, sendbuf, (int)strlen(sendbuf), 0, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));
    if (iResult == SOCKET_ERROR) {
        wprintf(L"sendto failed with error: %d\n", WSAGetLastError());
        closesocket(SendSocket);
        WSACleanup();
        return 1;
    }

    //----------------------------------------
    // Waiting the response from python server
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    char target[DEFAULT_BUFLEN];

    int nBytesRecv = recv(SendSocket, recvbuf, recvbuflen, 0);
    if (nBytesRecv > 0) {
        printf("Bytes received: %d\n", nBytesRecv);

        strncpy(target, recvbuf, nBytesRecv);
        target[nBytesRecv] = '\0';
        printf("Data received: %s\n", target);
    }
    else if (iResult == 0)
        printf("Connection closed\n");
    else
        printf("recv failed with error: %d\n", WSAGetLastError());

    //---------------------------------------------
    // When the application is finished sending, close the socket.
    wprintf(L"Finished sending. Closing socket.\n");
    iResult = closesocket(SendSocket);
    if (iResult == SOCKET_ERROR) {
        wprintf(L"closesocket failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    //---------------------------------------------
    // Clean up and quit.
    wprintf(L"Exiting.\n");
    WSACleanup();

    return 0;
}