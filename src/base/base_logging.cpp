variable_global String log_level_strings[Log_Level__MAX] = {
    #define X(_name, string, _color) STR(#string),
    LOG_LEVELS
    #undef X
};

function String log_level_name(Log_Level level) {
    return log_level_strings[level];
}

variable_global String log_level_colors[Log_Level__MAX] = {
    #define X(_name, _string, color) STR(color),
    LOG_LEVELS
    #undef X
};

function String log_level_color(Log_Level level) {
    return log_level_colors[level];
}

function void log_print_header(Log_Level level) {
    auto name = log_level_name(level);
    auto color = log_level_color(level);

    fwrite(color.data, cast(Uint)color.len, 1, stdout);
    fwrite(name.data, cast(Uint)name.len, 1, stdout);
    fputs(ANSI_RESET ": ", stdout);
}

function void log(Log_Level level, String message) {
    log_print_header(level);
    fwrite(message.data, cast(Uint)message.len, 1, stdout);
    fputs("\n", stdout);
}

function void vlogf(Log_Level level, char const *format, va_list list) {
    log_print_header(level);
    vprintf(format, list);
    fputs("\n", stdout);
}

#define X(name, string, _color) function void Glue(log_, string)(String message) { log(Glue(Log_Level_, name), message); } \
                                function void Glue(Glue(log_, string), f)(char const *format, ...) {\
                                    va_list list; \
                                    va_start(list, format);\
                                    vlogf(Glue(Log_Level_, name), format, list);\
                                    va_end(list);\
                                }
LOG_LEVELS
#undef X
