#include "base.h"
#include "internal.h"
#include "os.h"


void
#ifdef FL_LOG_EXTRA
log_printf_internal(char* format_string, char* f, uint l, ...)
#else
log_printf_internal(char* format_string, ...)
#endif // FL_LOG_EXTRA
    {
    va_list args;
    #ifdef FL_LOG_EXTRA
    va_start(args, l);
    #else
    va_start(args, format_string);
    #endif // FL_LOG_EXTRA

    char message_buffer[4096] = {0};
    vsnprintf(message_buffer, sizeof(message_buffer), format_string, args);

    String8 log_time = os_local_time_as_string8();
    #ifdef FL_LOG_EXTRA
    printf("[%s] <%s(%d)> %s", log_time.content, f, l, message_buffer);
    #else
    printf("[%s] %s", log_time.content, message_buffer);
    #endif // FL_LOG_EXTRA

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
            if (!(bytes_per_line % 2) && j == bytes_per_line / 2)
                {
                putchar(' ');
                }
            
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
