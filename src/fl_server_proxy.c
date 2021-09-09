#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

#include "base.h"

int
main(void)
    {
    WSADATA wsaData;
    int iResult;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR)
        {
        log_printf("[X] WSAStartup failed: %d\n", iResult);
        return 1;
        }
    log_printf("[*] WSAStartup succeeded.\n");


    struct addrinfo hints = {0};
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_flags    = AI_PASSIVE;

    struct addrinfo* result;
    iResult = getaddrinfo(0, "20042", &hints, &result);
    if (iResult != 0)
        {
        log_printf("[X] gettaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
        }
    log_printf("[*] gettaddrinfo succeeded.\n");


    SOCKET ListenSocket = INVALID_SOCKET;
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET)
        {
        log_printf("[X] Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
        }
    log_printf("[*] ListenSocket succeeded.\n");

    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
        {
        log_printf("[X] bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
        }
    log_printf("[*] bind succeeded.\n");

    fflush(stdout);

    u8 buffer[1024] = {0};
    for(;;)
        {
        iResult = recv(ListenSocket, buffer, 1024, 0);
        if (iResult == SOCKET_ERROR)
            {
            log_printf("[X] recv failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
            }
        util_byte_dump(buffer, iResult);
        fflush(stdout);
        }

    return 0;
    }
