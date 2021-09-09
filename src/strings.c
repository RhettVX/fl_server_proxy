#include "base.h"


//----------------------------------------------------------------
// Function definitions
//----------------------------------------------------------------
u32
strings_cstring_length(char* cstring)
    {
    u32 count = 0;
    while (*(cstring++))
        {
            ++count;
        }
    return count;
    }

String8
strings_cstring_to_string8(char* cstring)
    {
    String8 result = {0};
    result.length = strings_cstring_length(cstring);
    result.content = cstring;

    return result;
    }
