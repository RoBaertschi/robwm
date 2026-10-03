#include <stdint.h>
#include <inttypes.h>

typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef int8_t  I8;
typedef int16_t I16;
typedef int32_t I32;
typedef int64_t I64;

typedef U8  B8;
typedef U16 B16;
typedef U32 B32;
typedef U8 B64;

typedef B8 Bool;

typedef float  F32;
typedef double F64;

typedef I64 Int;
typedef U64 Uint;

typedef Uint Uintptr;

union V2I32 {
    struct {
        I32 x;
        I32 y;
    };
    I32 v[2];
};

enum Axis {
    Axis_X,
    Axis_Y,
    Axis__MAX,
};
