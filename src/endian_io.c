#include "base.h"


//// Little endian
u32
endian_get_u32_le(u8* data)
    {
    u32 result = 0;
    for (u32 i = 4; i > 0; --i)
        {
        result <<= 8;
        result |= *(data + (i - 1));
        }

    return result;
    }

u64
endian_get_u64_le(u8* data)
    {
    u64 result = 0;
    for (u64 i = 8; i > 0; --i)
        {
        result <<= 8;
        result |= *(data + (i - 1));
        }

    return result;
    }

//// Big endian
u16
endian_get_u16_be(u8* data)
    {
    u16 result = 0;
    for (u32 i = 0; i < 2; ++i)
        {
        result <<= 8;
        result |= *(data + i);
        }

    return result;
    }

u32
endian_get_u32_be(u8* data)
    {
    u32 result = 0;
    for (u32 i = 0; i < 4; ++i)
        {
        result <<= 8;
        result |= *(data + i);
        }

    return result;
    }
