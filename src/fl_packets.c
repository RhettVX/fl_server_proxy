#include "base.h"
#include "internal.h"
#include "os.h"

void
print_packet_type(u16 id)
    {
    switch (id)
        {
        case 0x0001:
            puts("SessionRequest");
            break;
        case 0x0002:
            puts("SessionReply");
            break;
        case 0x0003:
            puts("MultiPacket");
            break;
        case 0x0005:
            puts("Disconnect");
            break;
        case 0x0006:
            puts("Ping");
            break;
        case 0x0007:
            puts("NetStatusRequest");
            break;
        case 0x0008:
            puts("NetStatusReply");
            break;
        case 0x0009:
            puts("Data");
            break;
        case 0x000d:
            puts("DataFragment");
            break;
        case 0x0011:
            puts("OutOfOrder");
            break;
        case 0x0015:
            puts("Ack");
            break;
        case 0x001d:
            puts("FatalError");
            break;
        case 0x001e:
            puts("FatalErrorReply");
            break;
        default:
            puts("Unknown");
        }
    }
