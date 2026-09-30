#define ANSI_ESC "\033"
#define ANSI_RESET ANSI_ESC "[0m"

// X(name, string name, color)
#define LOG_LEVELS                      \
    X(Debug,   debug, ANSI_ESC "[0;90m")\
    X(Info,    info,  ANSI_ESC "[0;32m")\
    X(Warning, warn,  ANSI_ESC "[0;33m")\
    X(Error,   error, ANSI_ESC "[0;31m")

enum Log_Level {
    #define X(name, ...) Glue(Log_Level_, name),
    LOG_LEVELS
    #undef X
    Log_Level__MAX,
};

function String log_level_name(Log_Level level);
function String log_level_color(Log_Level level);
function void log(Log_Level level, String message);
function void vlogf(Log_Level level, char const *format, va_list list);

// bruh
#define X(name, string, _color) function void Glue(log_, string)(String message); \
                                function void Glue(Glue(log_, string), f)(char const *format, ...);
LOG_LEVELS
#undef X
