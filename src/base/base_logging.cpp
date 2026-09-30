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

variable_global Logger log_default_logger = {};

function void logger_print_header(Logger logger, Log_Level level) {
    auto name = log_level_name(level);
    auto color = log_level_color(level);

    fwrite(color.data, cast(Uint)color.len, 1, stdout);
    fwrite(name.data, cast(Uint)name.len, 1, stdout);
    fputs(ANSI_RESET, stdout);
    if (0 < logger.system.len) {
        fputs("(", stdout);
        fwrite(logger.system.data, cast(Uint)logger.system.len, 1, stdout);
        fputs(")", stdout);
    }
    fputs(": ", stdout);
}

function void logger_log(Logger logger, Log_Level level, String message) {
    logger_print_header(logger, level);
    fwrite(message.data, cast(Uint)message.len, 1, stdout);
    fputs("\n", stdout);
}

function void logger_vlogf(Logger logger, Log_Level level, char const *format, va_list list) {
    logger_print_header(logger, level);
    vprintf(format, list);
    fputs("\n", stdout);
}


#define X(name, string, _color) function void Glue(logger_, string)(Logger logger, String message) { logger_log(logger, Glue(Log_Level_, name), message); } \
                                function void Glue(Glue(logger_, string), f)(Logger logger, char const *format, ...) {\
                                    va_list list; \
                                    va_start(list, format);\
                                    logger_vlogf(logger, Glue(Log_Level_, name), format, list);\
                                    va_end(list);\
                                }
LOG_LEVELS
#undef X

function void log_print_header(Log_Level level) {
    auto name = log_level_name(level);
    auto color = log_level_color(level);

    fwrite(color.data, cast(Uint)color.len, 1, stdout);
    fwrite(name.data, cast(Uint)name.len, 1, stdout);
    fputs(ANSI_RESET ": ", stdout);
}

function void log(Log_Level level, String message) {
    logger_log(log_default_logger, level, message);
}

function void vlogf(Log_Level level, char const *format, va_list list) {
    logger_vlogf(log_default_logger, level, format, list);
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
