#pragma once

typedef enum {
    NULL_VARIABLE,
    MEMORY_ALLOCATION,
    OUT_OF_RANGE,
    EMPTY_LIST,
    NOT_FOUND,
    SOCKET,
    BIND,
    LISTEN,
    DATABASE,
    DATABASE_PREPARE,
    DATABASE_EXEC,
    DB_QUERY,
    JSON_PARSING,
    ENCODING_ERROR,
    THREAD_CREATION,
    CLIENT_ATTACH
} errors;

#define RED(m) "\033[01;31m"m"\033[0m"
#define GREEN(m) "\033[01;32m"m"\033[0m"
#define YELLOW(m) "\033[01;33m"m"\033[0m"
#define BLUE(m) "\033[01;34m"m"\033[0m"
#define MAGENTA(m) "\033[01;35m"m"\033[0m"
#define CYAN(m) "\033[01;36m"m"\033[0m"
#define WHITE(m) "\033[01;37m"m"\033[0m"

void throw_error(errors, const char *);

void debug_log(const char *fmt, ...);

void info_log(const char *fmt, ...);

int init_debug_log(void);

void close_debug_log(void);