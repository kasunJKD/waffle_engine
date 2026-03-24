#ifndef UTILS_H
#define UTILS_H

#include <cstdint>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef i32 bool32;
typedef i8 bool8;

typedef float real32;
typedef double real64;

#define internal static
#define global_variable static
#define persist static

#define globalconst static const 

#define bytes(n) (n)
#define Kbytes(n) (bytes(n) * 1024)
#define Mbytes(n) (Kbytes(n) * 1024)
#define Gbytes(n) (Mbytes((u64)n) * 1024) 
#define Tbytes(n) (Gbytes((u64)n) * 1024) 

#define S(X) #X
#define S_(X) S(X)
#define S__LINE__ S_(__LINE__)

#define LOG(msg) \
    printf(__FILE__ ":" S__LINE__ ": " msg "\n")

#define Bit_0 (1 << 0)
#define Bit_1 (1 << 1)
#define Bit_2 (1 << 2)
#define Bit_3 (1 << 3)
#define Bit_4 (1 << 4)
#define Bit_5 (1 << 5)
#define Bit_6 (1 << 6)
#define Bit_7 (1 << 7)
#define Bit_8 (1 << 8)

#define Byte_0 (0xffu)
#define Byte_1 (0xffu << 8)
#define Byte_2 (0xffu << 16)
#define Byte_3 (0xffu << 24)
#define Byte_4 (0xffu << 32)
#define Byte_5 (0xffu << 40)
#define Byte_6 (0xffu << 48)
#define Byte_7 (0xffu << 56)

#endif
