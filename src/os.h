#ifndef OS_H
#define OS_H


#define OS_MAX_PATH_LENGTH 256


typedef struct win32_socket win32_socket;


//// Memory functions
extern void* 
os_memory_alloc(u32 size);

extern b32
os_memory_free(void* pointer);

//// File functions
extern b32
os_load_entire_file(char* file_path, u8* buffer, u32 max_size);

extern b32
os_write_buffer_to_file(char* file_path, u8* buffer, u32 buffer_length);

extern b32
os_create_folder(char* folder_path);

//// Other functions
extern String8
os_local_time_as_string8();

//================================================================
#ifdef FL_WIN32
    #ifdef FL_DEBUG
        extern void* 
        win32_memory_alloc(u32 size, uint line, char* file);

        extern b32
        win32_memory_free(void* pointer, uint line, char* file);
    #else
        extern void* 
        win32_memory_alloc(u32 size);

        extern b32
        win32_memory_free(void* pointer);
    #endif // FL_DEBUG

    extern b32
    win32_load_entire_file(char* file_path, u8* buffer, u32 max_size);

    extern b32
    win32_write_buffer_to_file(char* file_path, u8* buffer, u32 buffer_length);

    extern b32
    win32_create_folder(char* folder_path);

    extern String8
    win32_local_time_as_string8();

    //// Networking
    extern win32_socket*
    win32_net_udp_listener_create(char* address, char* port);

    extern u32
    win32_net_udp_listener_recieve(win32_socket* socket, u8* buffer, u32 buffer_size);
#endif // FL_WIN32

//================================================================
#ifdef FL_DEBUG
    #define os_memory_alloc(sz) win32_memory_alloc(sz,__LINE__,__FILE__)
    #define os_memory_free(ptr) win32_memory_free(ptr,__LINE__,__FILE__)
#else
    #define os_memory_alloc(sz) win32_memory_alloc(sz)
    #define os_memory_free(ptr) win32_memory_free(ptr)
#endif // FL_DEBUG

#define os_load_entire_file(s,ptr,sz)     win32_load_entire_file(s,ptr,sz)
#define os_write_buffer_to_file(s,ptr,sz) win32_write_buffer_to_file(s,ptr,sz)
#define os_create_folder(s)               win32_create_folder(s)
#define os_local_time_as_string8()        win32_local_time_as_string8()  


#endif // OS_H
