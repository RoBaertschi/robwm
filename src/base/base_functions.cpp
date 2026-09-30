#include <stdarg.h>

function_noreturn void fail(char const *format, ...) {
    va_list list;

    va_start(list, format);
    vlogf(Log_Level_Error, format, list);
    va_end(list);

    AssertBreak();
}
