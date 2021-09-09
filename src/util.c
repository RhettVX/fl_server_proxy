#include "base.h"
#include "internal.h"
#include "os.h"


void
log_printf(char* format_string, ...)
    {
    va_list args;
    va_start(args, format_string);

    // char message_buffer[4096] = {0};
    // vsnprintf(message_buffer, sizeof(message_buffer), format_string, args);

    String8 log_time = win32_local_time_as_string();
    // printf("[%s] %s", log_time.content, message_buffer);

    va_end(args);
    }

// NOTE(rhett): Sloppy but does what I want
void
util_byte_dump(void* data, uint length)
    {
    uint bytes_per_line = 8;
    uint line_count = length / bytes_per_line;
    if (length % bytes_per_line)
        {
        ++line_count;
        }

    printf("\n==== Base: %p\n", data);
    for (uint i = 0; i < line_count; ++i)
        {
        printf("%08X: ", i*bytes_per_line);
        for (uint j = 0; j < bytes_per_line; ++j)
            {
            if (!(bytes_per_line % 2) && j == bytes_per_line / 2)
                {
                putchar(' ');
                }
            if (j < length - (i*bytes_per_line))
                {
                printf("%02x ", *(u8*)((uptr)data + (i*bytes_per_line+j)));
                }
            else
                {
                printf("   ");
                }
            }

        printf("| ");
        for (uint j = 0; j < bytes_per_line; ++j)
            {
            char byte = *(char*)((uptr)data + (i*bytes_per_line+j));
            if (byte < '!' || byte > '~')
                {
                byte = '.';
                }

            if (j < length - (i*bytes_per_line))
                {
                printf("%c", byte);
                }
            else
                {
                putchar(' ');
                }
            }
        printf(" |\n");
        }
    putchar('\n');

    return;
    }
