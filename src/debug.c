#ifdef FL_DEBUG


#include "base.h"
#include "internal.h"


global AllocationTable debug_allocation_table_global = {.count = 0,
                                                        .capacity = FL_DEBUG_MAX_ALLOCATIONS,
                                                        .entries = {0}};


void
debug_allocation_register(void* pointer, uint line, char* file)
    {
    if (debug_allocation_table_global.count == debug_allocation_table_global.capacity)
        {
        printf("[X] Exceeded max allocations! capacity=%u, count=%u; %s(%u)\n",
               debug_allocation_table_global.capacity,
               debug_allocation_table_global.count,
               file,
               line);

        // TODO(rhett): Not sure about this, but it should work for now
        abort();
        }

    AllocationEntry entry = {.pointer = pointer,
                             .has_been_freed = 0,
                             .origin_file = file,
                             .origin_line = line};

    debug_allocation_table_global.entries[debug_allocation_table_global.count] = entry;
    ++debug_allocation_table_global.count;
    return;
    }

void
debug_allocation_mark_as_freed(void* pointer, uint line, char* file)
    {
    // NOTE(rhett): Attempt to free pointer that hasn't been allocated
    AllocationEntry* entry = debug_allocation_get_by_pointer(pointer);
    if (entry == 0)
        {
        printf("[X] Attempting to free a pointer that has not been allocated! address=%p; %s(%u)\n",
               pointer,
               file,
               line);
        abort();
        }

    // NOTE(rhett): Attempt to free a pointer that has already been freed
    if (entry->has_been_freed)
        {
        printf("[X] Attempting to free a pointer that has already been freed! address=%p; %s(%u)\n",
               pointer,
               file,
               line);
        abort();
        }

    // NOTE(rhett): should be able to mark as freed now
    entry->has_been_freed = 1;
    }

AllocationEntry*
debug_allocation_get_by_pointer(void* pointer)
    {
    for (uint i = 0; i < debug_allocation_table_global.count; ++i)
        {
        if (debug_allocation_table_global.entries[i].pointer == pointer)
            {
            return &debug_allocation_table_global.entries[i];
            }
        }
    // TODO(rhett): Should this be a fatal error alone?
    printf("[!] Unable to get allocation entry by pointer! address=%p\n", pointer);
    return 0;
    }

void
debug_allocation_check_for_unfreed_memory()
    {
    for (uint i = 0; i < debug_allocation_table_global.count; ++i)
        {
        if (!debug_allocation_table_global.entries[i].has_been_freed)
            {
            printf("[!] Memory located at %p has not yet been freed!; %s(%u)\n",
                   debug_allocation_table_global.entries[i].pointer,
                   debug_allocation_table_global.entries[i].origin_file,
                   debug_allocation_table_global.entries[i].origin_line);

            return;
            }
        }
    }


#endif // FL_DEBUG
