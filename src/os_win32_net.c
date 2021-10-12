#ifdef FL_WIN32

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

#include "base.h"
#include "internal.h"
#include "os.h"

#define printf log_printf

#define LOCAL_ADDR  "127.0.0.1"
#define LOCAL_PORT  20042

#define SERVER_ADDR "lvspsn-tst-l01.planetside2.com"
#define SERVER_PORT 20042


struct win32_net_socket
    {
    b32    is_valid;
    SOCKET socket;
    };

struct win32_net_address
    {
    b32       is_valid;
    // TODO(rhett): I don't like using this bool here
    b32 use_freeaddrinfo;
    ADDRINFO* address;
    };


global b32 is_wsa_initialized = FALSE;


b32
win32_net_wsa_startup()
    {
    if (is_wsa_initialized)
        {
        printf("WSA already initialized\n");
        return is_wsa_initialized;
        }
    WSADATA wsa_data;
    int error = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (error)
        {
        printf("WSAStartup failed: %d\n", error);
        return FALSE;
        }
    return is_wsa_initialized = TRUE;
    }

void
win32_net_wsa_cleanup()
    {
    if (!is_wsa_initialized)
        {
        printf("WSA not initialized\n");
        return;
        }
    is_wsa_initialized = FALSE;
    WSACleanup();
    }

// TODO(rhett): Give more options for how to allocate this
win32_net_socket*
win32_net_socket_create_udp()
    {
    win32_net_socket* result = malloc(sizeof(*result));
    result->socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (result->socket == INVALID_SOCKET)
        {
        printf("socket failed: %d\n", WSAGetLastError());
        return result;
        }
    result->is_valid = TRUE;
    return result;
    }

void
win32_net_socket_destroy(win32_net_socket* socket)
    {
    closesocket(socket->socket);
    free(socket);
    }

b32
win32_net_socket_check_is_valid(win32_net_socket* socket)
    {
    return socket->is_valid;
    }

b32
win32_net_socket_bind(win32_net_socket* socket, char* address, u16 port)
    {
    SOCKADDR_IN addr = {.sin_family      = AF_INET,
                        .sin_port        = htons(port),
                        .sin_addr.s_addr = inet_addr(address)};
    int bind_result = bind(socket->socket, (SOCKADDR*)&addr, sizeof(addr));
    if (bind_result == SOCKET_ERROR)
        {
        printf("bind failed: %d\n", WSAGetLastError());
        return FALSE;
        }
    return TRUE;
    }

win32_net_address*
win32_net_address_create_and_resolve_udp(char* address, char* port)
    {
    win32_net_address* result = malloc(sizeof(*result));
    ADDRINFO hints = {.ai_family   = AF_INET,
                      .ai_socktype = SOCK_DGRAM,
                      .ai_protocol = IPPROTO_UDP};

    int getaddrinfo_result = getaddrinfo(address, port, &hints, &result->address);
    if (getaddrinfo_result)
        {
        printf("getaddrinfo failed: %d\n", WSAGetLastError());
        return result;
        }
    result->is_valid = TRUE;
    result->use_freeaddrinfo = TRUE;
    return result;
    }

win32_net_address*
win32_net_address_create()
    {
    win32_net_address* result = malloc(sizeof(*result) + sizeof(ADDRINFO) + sizeof(SOCKADDR));
    result->address           = (ADDRINFO*)((uptr)result + sizeof(*result));
    result->address->ai_addr  = (SOCKADDR*)((uptr)result + sizeof(*result) + sizeof(ADDRINFO));

    result->address->ai_addrlen = sizeof(SOCKADDR);
    return result;
    }

void
win32_net_address_destroy(win32_net_address* address)
    {
    if (address->use_freeaddrinfo)
        {
        freeaddrinfo(address->address);
        }
    free(address);
    }

b32
win32_net_address_check_is_valid(win32_net_address* address)
    {
    return address->is_valid;
    }

b32
win32_net_socket_connect(win32_net_socket* socket, win32_net_address* address)
    {
    if (connect(socket->socket, address->address->ai_addr, address->address->ai_addrlen) == SOCKET_ERROR)
        {
        printf("connect failed: %d\n", WSAGetLastError());
        return FALSE;
        }
    return TRUE;
    }

u32
win32_net_recieve_from(win32_net_socket* socket, win32_net_address* from_address, u8* buffer, u32 buffer_length)
    {
    int from_address_length = sizeof(*from_address->address);
    int recvfrom_result = recvfrom(socket->socket, buffer, buffer_length, 0,
                                   from_address->address->ai_addr, &from_address_length);
    if (recvfrom_result == SOCKET_ERROR)
        {
        printf("recvfrom failed: %d\n", WSAGetLastError());
        return 0;
        }
    return recvfrom_result;
    }

u32
win32_net_send_to(win32_net_socket* socket, win32_net_address* from_address, u8* buffer, u32 buffer_length)
    {
    int sendto_result = sendto(socket->socket, buffer, buffer_length, 0,
                               from_address->address->ai_addr, from_address->address->ai_addrlen);
    if (sendto_result == SOCKET_ERROR)
        {
        printf("sendto failed: %d\n", WSAGetLastError());
        return 0;
        } 
    return sendto_result;
    }

u32
win32_net_recieve(win32_net_socket* socket, u8* buffer, u32 buffer_length)
    {
    int recv_result = recv(socket->socket, buffer, buffer_length, 0);
    if (recv_result == SOCKET_ERROR)
        {
        printf("recv failed: %d\n", WSAGetLastError());
        return 0;
        }
    return recv_result;
    }

u32
win32_net_send(win32_net_socket* socket, u8* buffer, u32 buffer_length)
    {
    int send_result = send(socket->socket, buffer, buffer_length, 0);
    if (send_result == SOCKET_ERROR)
        {
        printf("send failed: %d\n", WSAGetLastError());
        return 0;
        }
    return send_result;
    }

void
win32_net_test_function()
    {
    if (!is_wsa_initialized)
        {
        if (!win32_net_wsa_startup())
            {
            abort();
            }
        }

    //----------------------------------------------------------------
    // Client listener
    //----------------------------------------------------------------
    win32_net_socket* client_listener_socket = win32_net_socket_create_udp();
    if (client_listener_socket->is_valid == FALSE)
        {
        goto abort;
        }

    if (!win32_net_socket_bind(client_listener_socket, LOCAL_ADDR, LOCAL_PORT))
        {
        win32_net_socket_destroy(client_listener_socket);
        goto abort;
        }
    //================================================================

    //----------------------------------------------------------------
    // Server sender
    //----------------------------------------------------------------
    win32_net_address* server_sender_address = win32_net_address_create_and_resolve_udp(SERVER_ADDR, STRINGIFY(SERVER_PORT));
    if (!server_sender_address->is_valid)
        {
        win32_net_socket_destroy(client_listener_socket);
        goto abort;
        }

    win32_net_socket* server_sender_socket = win32_net_socket_create_udp();
    if (!server_sender_socket->is_valid)
        {
        win32_net_socket_destroy(client_listener_socket);
        win32_net_address_destroy(server_sender_address);
        goto abort;
        }

    if (!win32_net_socket_connect(server_sender_socket, server_sender_address))
        {
        win32_net_socket_destroy(client_listener_socket);
        win32_net_address_destroy(server_sender_address);
        win32_net_socket_destroy(server_sender_socket);
        goto abort;
        }
    //================================================================

    u8 buffer[512] = {0};

    win32_net_address* from_addr = win32_net_address_create();
    int recv_result = win32_net_recieve_from(client_listener_socket, from_addr,
                                             buffer, sizeof(buffer));
    if (!recv_result)
        {
        printf("[X] Unable to receive data.\n");
        win32_net_socket_destroy(client_listener_socket);
        win32_net_address_destroy(server_sender_address);
        win32_net_socket_destroy(server_sender_socket);
        win32_net_address_destroy(from_addr);
        goto abort;
        }
    printf("[*] Received %d bytes\n", recv_result);
    util_byte_dump(buffer, recv_result);

    printf("[*] Forwarding...\n");
    int send_result = win32_net_send(server_sender_socket, buffer, recv_result);
    if (!send_result)
        {
        printf("[X] Failed to send data.\n");
        win32_net_socket_destroy(client_listener_socket);
        win32_net_address_destroy(server_sender_address);
        win32_net_socket_destroy(server_sender_socket);
        win32_net_address_destroy(from_addr);
        goto abort;
        }
    printf("[*] Sent %d bytes\n", send_result);

    recv_result = win32_net_recieve(server_sender_socket, buffer, sizeof(buffer));
    if (!recv_result)
        {
        printf("[X] Failed to receive data from server.\n");
        win32_net_socket_destroy(client_listener_socket);
        win32_net_address_destroy(server_sender_address);
        win32_net_socket_destroy(server_sender_socket);
        win32_net_address_destroy(from_addr);
        goto abort;
        }
    printf("[*] Received %d bytes\n", recv_result);
    util_byte_dump(buffer, recv_result);

    printf("[*] Forwarding...\n");
    send_result = win32_net_send_to(client_listener_socket, from_addr, buffer, recv_result);
    if (!send_result)
        {
        printf("[X] Unable to send to client.\n");
        win32_net_socket_destroy(client_listener_socket);
        win32_net_address_destroy(server_sender_address);
        win32_net_socket_destroy(server_sender_socket);
        win32_net_address_destroy(from_addr);
        goto abort;
        }
    printf("[*] Sent %d bytes\n", send_result);

    recv_result = win32_net_recieve(client_listener_socket, buffer, sizeof(buffer));
    if (!recv_result)
        {
        printf("[X] Unable to receive from client.\n");
        win32_net_socket_destroy(client_listener_socket);
        win32_net_address_destroy(server_sender_address);
        win32_net_socket_destroy(server_sender_socket);
        win32_net_address_destroy(from_addr);
        goto abort;
        }
    printf("[*] Received %d bytes\n", recv_result);
    util_byte_dump(buffer, recv_result);

    WSACleanup();
    return;
abort_server_socket:
    win32_net_socket_destroy(server_sender_socket);
abort_server_addr:
    win32_net_address_destroy(server_sender_address);
abort_client_socket:
    win32_net_socket_destroy(client_listener_socket);
abort:
    WSACleanup();
    abort();
    }


#endif //FL_WIN32
