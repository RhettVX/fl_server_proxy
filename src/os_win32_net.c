#ifdef FL_WIN32

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

#include "base.h"
#include "internal.h"
#include "os.h"


// typedef struct win32_socket win32_socket;
struct win32_socket
    {
    SOCKET socket;
    };


global b32 is_wsa_initialized = 0;


void
win32_net_wsa_startup()
    {
    if (is_wsa_initialized)
        {
        log_printf("[!] WSA already initialized.\n");
        return;
        }

    WSADATA wsa_data;
    int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (result != NO_ERROR)
        {
        log_printf("[X] WSAStartup failed: %d\n", result);
        abort();
        }
    is_wsa_initialized = 1;
    }

win32_socket*
win32_net_udp_listener_create(char* address, char* port)
    {
    if (!is_wsa_initialized)
        {
        win32_net_wsa_startup();
        }

    struct addrinfo hints = {.ai_family   = AF_INET,
                             .ai_socktype = SOCK_DGRAM,
                             .ai_protocol = IPPROTO_UDP,
                             .ai_flags    = AI_PASSIVE};

    struct addrinfo* addr_result;
    int result = getaddrinfo(address, port, &hints, &addr_result);
    if (result != 0)
        {
        log_printf("[X] getaddrinfo failed: %d\n", result);
        WSACleanup();
        abort();
        }

    SOCKET listen_socket = INVALID_SOCKET;
    listen_socket = socket(addr_result->ai_family, addr_result->ai_socktype, addr_result->ai_protocol);
    if (listen_socket == INVALID_SOCKET)
        {
        log_printf("[X] Error creating socket: %d\n", WSAGetLastError());
        freeaddrinfo(addr_result);
        WSACleanup();
        abort();
        }

    result = bind(listen_socket, addr_result->ai_addr, addr_result->ai_addrlen);
    if (result == SOCKET_ERROR)
        {
        log_printf("[X] bind failed: %d\n", WSAGetLastError());
        freeaddrinfo(addr_result);
        closesocket(listen_socket);
        WSACleanup();
        abort();
        }

    local_persist win32_socket return_result = {0};
    return_result.socket = listen_socket;
    return &return_result;
    }

u32
win32_net_udp_listener_recieve(win32_socket* socket, u8* buffer, u32 buffer_size)
    {
    int result = recv(socket->socket, buffer, buffer_size, 0);
    if (result == SOCKET_ERROR)
        {
        log_printf("[X] recv failed: %d\n", WSAGetLastError());
        closesocket(socket->socket);
        WSACleanup();
        abort();
        }
    return result;
    }

#endif //FL_WIN32
