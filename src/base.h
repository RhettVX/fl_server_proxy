#ifndef BASE_H
#define BASE_H


//----------------------------------------------------------------
// Defines
//----------------------------------------------------------------
#define internal      static
#define local_persist static
#define global        static
#define dll_external  extern __declspec(dllexport)

#define TRUE  1
#define FALSE 0

#define KB(n) (n << 10)
#define MB(n) (n << 20)
#define GB(n) ((u64)n << 30)
#define TB(n) ((u64)n << 40)

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)

#include <stdio.h>
#define EVAL_PRINT_U8(x)   printf("%s = %u:%xh\n",#x,(u8)x,(u8)x)
#define EVAL_PRINT_I8(x)   printf("%s = %d:%xh\n",#x,(i8)x,(i8)x)
#define EVAL_PRINT_U16(x)  printf("%s = %u:%xh\n",#x,(u16)x,(u16)x)
#define EVAL_PRINT_I16(x)  printf("%s = %d:%xh\n",#x,(i16)x,(i16)x)
#define EVAL_PRINT_U32(x)  printf("%s = %u:%xh\n",#x,(u32)x,(u32)x)
#define EVAL_PRINT_I32(x)  printf("%s = %d:%xh\n",#x,(i32)x,(i32)x)
#define EVAL_PRINT_U64(x)  printf("%s = %llu:%llxh\n",#x,(u64)x,(u64)x)
#define EVAL_PRINT_I64(x)  printf("%s = %lld:%llxh\n",#x,(i64)x,(i64)x)
#define EVAL_PRINT_CSTR(x) printf("%s = \"%s\"\n",#x,x)

#define ARRAY_COUNT(a) sizeof(a) / sizeof(a[0])
#define STRINGIFY_(x)  #x
#define STRINGIFY(x)   STRINGIFY_(x)


//================================================================
#include <stdlib.h>
#include <string.h>

#define malloc(sz)           os_memory_alloc(sz)
#define free(ptr)            os_memory_free(ptr)
#define memcpy(src,dst,sz)   memcpy(dst,src,sz)
#define memcmp(ptr1,ptr2,sz) memcmp(ptr1,ptr2,sz)
#define memset(ptr,val,sz)   memset(ptr,val,sz)


//----------------------------------------------------------------
// Type aliases
//----------------------------------------------------------------
#include <stdint.h>

typedef   int8_t i8;
typedef  int16_t i16;
typedef  int32_t i32;
typedef  int64_t i64;
typedef intptr_t iptr;

typedef      uint8_t u8;
typedef     uint16_t u16;
typedef     uint32_t u32;
typedef     uint64_t u64;
typedef    uintptr_t uptr;
typedef unsigned int uint;

typedef  i8 b8;
typedef i16 b16;
typedef i32 b32;
typedef i64 b64;

typedef  float f32;
typedef double f64;

typedef struct String8 String8;
struct String8
    {
    u32 length;
    u8* content;
    };


#endif // BASE_H
