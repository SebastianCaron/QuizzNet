#pragma once

typedef enum {
    MEMORY_ALLOCATION
} errors;

#define RED(m) "\033[01;31m"m"\033[0m"
#define GREEN(m) "\033[01;32m"m"\033[0m"
#define YELLOW(m) "\033[01;33m"m"\033[0m"
#define BLUE(m) "\033[01;34m"m"\033[0m"
#define MAGENTA(m) "\033[01;35m"m"\033[0m"
#define CYAN(m) "\033[01;36m"m"\033[0m"
#define WHITE(m) "\033[01;37m"m"\033[0m"

void throw_error(errors, char **);

void debug_log(char *);