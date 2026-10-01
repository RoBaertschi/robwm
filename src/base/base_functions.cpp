#include <stdarg.h>

function_noreturn void fail(char const *format, ...) {
    va_list list;

    va_start(list, format);
    vlogf(Log_Level_Error, format, list);
    va_end(list);

    AssertBreak();
}

function U64 mix_u64(U64 x) {
    // Based on https://stackoverflow.com/a/12996028
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9_u64;
    x = (x ^ (x >> 27)) * 0x94d049bb133111eb_u64;
    x = x ^ (x >> 31);
    return x;
}

function U64 unmix_u64(U64 x) {
    x = (x ^ (x >> 31) ^ (x >> 62)) * 0x319642b2d24d8ec3_u64;
    x = (x ^ (x >> 27) ^ (x >> 54)) * 0x96de1b173f119089_u64;
    x = x ^ (x >> 30) ^ (x >> 60);
    return x;
}
