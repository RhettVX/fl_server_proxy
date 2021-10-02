#ifdef FL_WIN32

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

#include "base.h"
#include "internal.h"
#include "os.h"

#define LOCAL_ADDR  "127.0.0.1"
#define LOCAL_PORT  20042

#define SERVER_ADDR "lvspsn-tst-l01.planetside2.com"
#define SERVER_PORT 20042


void
win32_net_test_function()
    {
    WSADATA wsa_data;
    int wsa_startup_result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (wsa_startup_result)
        {
        log_printf("[X] WSAStartup failed: %d\n", wsa_startup_result);
        goto abort;
        }

    //----------------------------------------------------------------
    // Client listener
    //----------------------------------------------------------------
    SOCKET client_listener_socket;
    client_listener_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (client_listener_socket == INVALID_SOCKET)
        {
        log_printf("[X] socket failed: %d\n", WSAGetLastError());
        goto abort;
        }

    SOCKADDR_IN client_listener_addr = {.sin_family      = AF_INET,
                                        .sin_port        = htons(LOCAL_PORT),
                                        .sin_addr.s_addr = inet_addr(LOCAL_ADDR)};
    int bind_result = bind(client_listener_socket,
                           (SOCKADDR*)&client_listener_addr, sizeof(client_listener_addr));
    if (bind_result == SOCKET_ERROR)
        {
        log_printf("[X] bind failed: %d\n", WSAGetLastError());
        goto abort_client_socket;
        }
    //================================================================

    //----------------------------------------------------------------
    // Server sender
    //----------------------------------------------------------------
    ADDRINFO* server_sender_addr;
    ADDRINFO hints = {.ai_family   = AF_INET,
                      .ai_socktype = SOCK_DGRAM,
                      .ai_protocol = IPPROTO_UDP};
    int getaddrinfo_result = getaddrinfo(SERVER_ADDR, STRINGIFY(SERVER_PORT), &hints, &server_sender_addr);
    if (getaddrinfo_result)
        {
        log_printf("[X] getaddrinfo failed: %d\n", getaddrinfo_result);
        goto abort_client_socket;
        }

    SOCKET server_sender_socket;
    server_sender_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (server_sender_socket == INVALID_SOCKET)
        {
        log_printf("[X] socket failed: %d\n", WSAGetLastError());
        goto abort_server_addr;
        }

    int connect_result = connect(server_sender_socket,
                                 server_sender_addr->ai_addr, server_sender_addr->ai_addrlen);
    if (connect_result == SOCKET_ERROR)
        {
        log_printf("[X] connect failed: %d\n", WSAGetLastError());
        goto abort_server_socket;
        }
    //================================================================

    //----------------------------------------------------------------
    // Server listener
    //----------------------------------------------------------------
    u8 buffer[512] = {0};

    SOCKADDR from_addr;
    int from_length = sizeof(from_addr);
    int recv_result = recvfrom(client_listener_socket, buffer, sizeof(buffer), 0, &from_addr, &from_length);
    if (recv_result == SOCKET_ERROR)
        {
        log_printf("[X] recvfrom failed: %d\n", WSAGetLastError());
        goto abort_server_socket;
        }
    log_printf("[*] Recieved %d bytes\n", recv_result);
    util_byte_dump(buffer, recv_result);

    log_printf("[*] Forwarding...\n");
    int send_result = send(server_sender_socket, buffer, recv_result, 0);
    if (send_result == SOCKET_ERROR)
        {
        log_printf("[X] send failed: %d\n", WSAGetLastError());
        goto abort_server_socket;
        }
    log_printf("[*] Sent %d bytes\n", send_result);

    recv_result = recv(server_sender_socket, buffer, sizeof(buffer), 0);
    if (recv_result == SOCKET_ERROR)
        {
        log_printf("[X] recv failed: %d\n", WSAGetLastError());
        goto abort_server_socket;
        }
    log_printf("[*] Recieved %d bytes\n", recv_result);
    util_byte_dump(buffer, recv_result);

    log_printf("[*] Forwarding...\n");
    send_result = sendto(client_listener_socket, buffer, recv_result, 0, &from_addr, from_length);
    if (send_result == SOCKET_ERROR)
        {
        log_printf("[X] sendto failed: %d\n", WSAGetLastError());
        goto abort_server_socket;
        }
    log_printf("[*] Sent %d bytes\n", send_result);

    recv_result = recv(client_listener_socket, buffer, sizeof(buffer), 0);
    if (recv_result == SOCKET_ERROR)
        {
        log_printf("[X] recv failed: %d\n", WSAGetLastError());
        goto abort_server_socket;
        }
    log_printf("[*] Recieved %d bytes\n", recv_result);
    util_byte_dump(buffer, recv_result);

    WSACleanup();
    return;
abort_server_socket:
    closesocket(server_sender_socket);
abort_server_addr:
    freeaddrinfo(server_sender_addr);
abort_client_socket:
    closesocket(client_listener_socket);
abort:
    WSACleanup();
    abort();
    }


#endif //FL_WIN32
