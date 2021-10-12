#ifndef OS_H
#define OS_H


#define OS_MAX_PATH_LENGTH 256

#ifdef FL_WIN32
    // TODO(rhett): This works for now
    // NOTE(rhett): Example: 127, 0, 0, 1, 80 
    #define WIN32_SINADDR_AS_ARGS(addr) addr.sin_addr.S_un.S_un_b.s_b1,\
                                        addr.sin_addr.S_un.S_un_b.s_b2,\
                                        addr.sin_addr.S_un.S_un_b.s_b3,\
                                        addr.sin_addr.S_un.S_un_b.s_b4,\
                                        ntohs(addr.sin_port)

    typedef struct win32_net_socket  win32_net_socket;
    typedef struct win32_net_address win32_net_address;
#endif // FL_WIN32


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
    extern b32
    win32_net_wsa_startup();

    extern void
    win32_net_wsa_cleanup();

    extern win32_net_socket*
    win32_net_socket_create_udp();

    extern void
    win32_net_socket_destroy(win32_net_socket* socket);

    extern b32
    win32_net_socket_check_is_valid(win32_net_socket* socket);

    extern b32
    win32_net_socket_bind(win32_net_socket* socket, char* address, u16 port);

    extern win32_net_address*
    win32_net_address_create_and_resolve_udp(char* address, char* port);

    extern win32_net_address*
    win32_net_address_create();

    extern void
    win32_net_address_destroy(win32_net_address* address);

    extern b32
    win32_net_address_check_is_valid(win32_net_address* address);

    extern b32
    win32_net_socket_connect(win32_net_socket* socket, win32_net_address* address);

    extern u32
    win32_net_recieve_from(win32_net_socket* socket, win32_net_address* from_address, u8* buffer, u32 buffer_length);

    extern u32
    win32_net_send_to(win32_net_socket* socket, win32_net_address* from_address, u8* buffer, u32 buffer_length);

    extern u32
    win32_net_recieve(win32_net_socket* socket, u8* buffer, u32 buffer_length);

    extern u32
    win32_net_send(win32_net_socket* socket, u8* buffer, u32 buffer_length);
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
