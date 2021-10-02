#include "base.h"
#include "internal.h"
#include "os.h"

#define PACKET_LENGTH 512

#define printf log_printf

int
main(void)
    {
    printf("[*] Waiting for initial packet from client...\n");
    win32_net_test_function();
    
    return 0;
    }
