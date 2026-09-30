#include <stdarg.h>

function_noreturn void fail(char const *format, ...) {
    va_list list;
    va_start(list, format);

    vprintf(format, list);
    puts("\n");
    AssertBreak();
}
