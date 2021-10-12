#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>

#include "base.h"
#include "internal.h"
#include "os.h"

#define PACKET_LENGTH 512

#define LOCAL_ADDR  "127.0.0.1"
#define LOCAL_PORT  20042

#define SERVER_ADDR "lvspsn-tst-l01.planetside2.com"
#define SERVER_PORT 20042

#define printf log_printf


struct proxy_client_args
    {
    win32_net_socket* client_communication_socket;
    win32_net_socket* server_communication_socket;
    };

struct proxy_server_args
    {
    win32_net_socket*  server_communication_socket;
    win32_net_address* game_client_address;
    };

void
proxy_client_connection(struct proxy_client_args* args)
    {
    u8 buffer[PACKET_LENGTH] = {0};
    for (;;)
        {
// TODO(rhett): Check if this label needs to be before the for loop
start:
        u32 recv_length = win32_net_recieve(args->client_communication_socket, buffer, sizeof(buffer));
        if (!recv_length)
            {
            printf("[!] Error receiving packs from client\n");
            goto start;
            }
        printf("[*] Received %d bytes from client\n", recv_length);
        util_byte_dump(buffer, recv_length);

        u32 send_length = win32_net_send(args->server_communication_socket, buffer, recv_length);
        if (!send_length)
            {
            printf("[!] Error sending packet to client\n");
            goto start;
            }
        printf("[*] Forwarded %d bytes to server\n", send_length);
        }
    }

void
proxy_server_connection(struct proxy_server_args* args)
    {
    u8 buffer[PACKET_LENGTH] = {0};
    for (;;)
        {
// TODO(rhett): Check if this label needs to be before the for loop
start:
        u32 recv_length = win32_net_recieve(args->server_communication_socket, buffer, sizeof(buffer));
        if (!recv_length)
            {
            printf("[!] Error receiving packet from server\n");
            goto start;
            }
        printf("[*] Received %d bytes from server\n", recv_length);
        util_byte_dump(buffer, recv_length);

        u32 send_length = win32_net_send_to(args->server_communication_socket, args->game_client_address, buffer, recv_length);
        if (!send_length)
            {
            printf("[!] Error sending packet to client\n");
            goto start;
            }
        printf("[*] Forwarded %d bytes to client.\n", send_length);
        }
    }

int
main(void)
    {
    if (!win32_net_wsa_startup())
        {
        return 1;
        }

    //// Client
    win32_net_socket* client_communication_socket = win32_net_socket_create_udp();
    if (!win32_net_socket_check_is_valid(client_communication_socket))
        {
        goto abort_wsa;
        }

    if (!win32_net_socket_bind(client_communication_socket, LOCAL_ADDR, LOCAL_PORT))
        {
        goto abort_client_socket;
        }
    printf("[*] Client listener bound to " LOCAL_ADDR ":" STRINGIFY(LOCAL_PORT) "\n");

    //// Server
    win32_net_address* server_address = win32_net_address_create_and_resolve_udp(SERVER_ADDR, STRINGIFY(SERVER_PORT));
    if (!win32_net_address_check_is_valid(server_address))
        {
        goto abort_client_socket;
        }

    win32_net_socket* server_communication_socket = win32_net_socket_create_udp();
    if (!win32_net_socket_check_is_valid(server_communication_socket))
        {
        goto abort_server_addr;
        }

    if (!win32_net_socket_connect(server_communication_socket, server_address))
        {
        goto abort_server_socket;
        }

    win32_net_address* game_client_address = win32_net_address_create();
    os_create_folder("packets");

    // NOTE(rhett): Get the address of the game client first
    u8 buffer[PACKET_LENGTH] = {0};
    u32 recv_length = win32_net_recieve_from(client_communication_socket, game_client_address, buffer, sizeof(buffer));
    if (!recv_length)
        {
        goto abort_recv_address;
        }

    struct proxy_client_args client_args = {.client_communication_socket = client_communication_socket,
                                .server_communication_socket = server_communication_socket};

    struct proxy_server_args server_args = {.server_communication_socket = server_communication_socket,
                                .game_client_address         = game_client_address};

    printf("Thread 1:\n");
    HANDLE client_thread = (HANDLE)_beginthread(proxy_client_connection, 0, &client_args);
    printf("Thread 2:\n");
    HANDLE server_thread = (HANDLE)_beginthread(proxy_server_connection, 0, &server_args);
    WaitForSingleObject(client_thread, INFINITE);
    WaitForSingleObject(server_thread, INFINITE);

    // for (;;)
    //     {
    //     u32 recv_length = win32_net_recieve_from(client_communication_socket, game_client_address,
    //                                              buffer, sizeof(buffer));
    //     if (!recv_length)
    //         {
    //         goto abort_recv_address;
    //         }
    //     printf("[*] Received %d bytes from game client.\n", recv_length);
    //     util_byte_dump(buffer, recv_length);
    //     // local_persist char dump_name[128] = {0};
    //     // sprintf(dump_name, "packets\\c_packet_%d.bin", packet_counter++);
    //     // os_write_buffer_to_file(dump_name, buffer, recv_length);
    //     // memset(buffer, 0, sizeof(buffer));

    //     u32 send_length = win32_net_send(server_communication_socket,
    //                                      buffer, recv_length);
    //     if (!send_length)
    //         {
    //         goto abort_recv_address;
    //         }
    //     printf("[*] Forwarded %d bytes to server.\n", send_length);

    //     recv_length = win32_net_recieve(server_communication_socket,
    //                                     buffer, sizeof(buffer));
    //     printf("[*] Received %d bytes from server.\n", recv_length);
    //     util_byte_dump(buffer, recv_length);
    //     // sprintf(dump_name, "packets\\s_packet_%d.bin", packet_counter++);
    //     // os_write_buffer_to_file(dump_name, buffer, recv_length);
    //     // memset(buffer, 0, sizeof(buffer));

    //     send_length = win32_net_send_to(client_communication_socket, game_client_address,
    //                                     buffer, recv_length);
    //     if (!send_length)
    //         {
    //         goto abort_recv_address;
    //         }
    //     printf("[*] Forwarded %d bytes to game client.\n", recv_length);
    //     }

    goto abort_recv_address;
    debug_allocation_check_for_unfreed_memory();
    return 0;
abort_recv_address:
    win32_net_address_destroy(game_client_address);
abort_server_socket:
    win32_net_socket_destroy(server_communication_socket);
abort_server_addr:
    win32_net_address_destroy(server_address);
abort_client_socket:
    win32_net_socket_destroy(client_communication_socket);
abort_wsa:
    win32_net_wsa_cleanup();
    debug_allocation_check_for_unfreed_memory();
    return 1;
    }
