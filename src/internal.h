#ifndef INTERNAL_H
#define INTERNAL_H

#include <stdarg.h>


//----------------------------------------------------------------
// Strings
//----------------------------------------------------------------
extern u32
strings_cstring_length(char* cstring);

extern String8
strings_cstring_to_string8(char* cstring);


//----------------------------------------------------------------
// Endian IO
//----------------------------------------------------------------
//// Little endian
extern u32
endian_get_u32_le(u8* data);

extern u64
endian_get_u64_le(u8* data);

//// Big endian
extern u16
endian_get_u16_be(u8* data);

extern u32
endian_get_u32_be(u8* data);


//----------------------------------------------------------------
// Util
//----------------------------------------------------------------
extern void
#ifdef FL_LOG_EXTRA
log_printf_internal(char* format_string, char* f, uint l, ...);
#else
log_printf_internal(char* format_string, ...);
#endif // FL_LOG_EXTRA

#ifdef FL_LOG_EXTRA
#define log_printf(x, ...) log_printf_internal(x, __FILE__, __LINE__, __VA_ARGS__)
#else
#define log_printf(x, ...) log_printf_internal(x, __VA_ARGS__)
#endif // FL_LOG_EXTRA

extern void
util_byte_dump(void* data, uint length);


//----------------------------------------------------------------
// Debug
//----------------------------------------------------------------
#ifdef FL_DEBUG

    #define FL_DEBUG_MAX_ALLOCATIONS 256

    typedef struct AllocationEntry AllocationEntry;
    struct AllocationEntry
        {
        void* pointer;
        b32   has_been_freed;
        char* origin_file;
        uint  origin_line;
        };

    typedef struct AllocationTable AllocationTable;
    struct AllocationTable
        {
        uint            count;
        uint            capacity;
        AllocationEntry entries[FL_DEBUG_MAX_ALLOCATIONS];
        };

    // extern AllocationTable debug_allocation_table_global;

    extern void
    debug_allocation_register(void* pointer, uint line, char* file);

    extern void
    debug_allocation_mark_as_freed(void* pointer, uint line, char* file);

    extern AllocationEntry*
    debug_allocation_get_by_pointer(void* pointer);

    // NOTE(rhett): Also externed in forgelight_lib.h
    extern void
    debug_allocation_check_for_unfreed_memory();
    
#endif // FL_DEBUG


#endif // INTERNAL_H
