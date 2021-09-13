#include "base.h"
#include "internal.h"
#include "os.h"

int
main(void)
    {
    win32_socket* socket = win32_net_udp_listener_create(0, "20042");
    log_printf("[*] Udp listener created.\n");

    u8 buffer[1024] = {0};
    for (;;)
        {
        u32 size = win32_net_udp_listener_recieve(socket, buffer, 1024);
        util_byte_dump(buffer, size);
        fflush(stdout);
        }

    return 0;
    }
