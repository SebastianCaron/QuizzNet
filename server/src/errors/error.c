#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>
#include <pthread.h>

#include "error.h"

/* Log file handle and mutex for thread-safe writing */
static FILE *log_file = NULL;
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief Internal enum for log message types.
 */
typedef enum{
    DEBUG,
    INFO,
    ERROR
} print_type;

int init_debug_log(void) {
    /* Generate filename with current timestamp */
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    char filename[64];
    strftime(filename, sizeof(filename), "%Y_%m_%d_%H_%M_%S.log", t);

    log_file = fopen(filename, "a");
    if (!log_file) {
        perror("fopen");
        return -1;
    }
    return 0;
}

void close_debug_log(void) {
    if (log_file) {
        fclose(log_file);
        log_file = NULL;
    }
}

/**
 * @brief Prints the log prefix to console with colored tag and timestamp.
 * 
 * @param type The type of message (DEBUG, INFO, or ERROR).
 */
void print_console(print_type type){
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_buf[32];
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", t);

    switch (type)
    {
    case DEBUG:
        printf(WHITE("[")YELLOW("DEBUG")WHITE("]\t[%s] "), time_buf);
        break;
    case ERROR:
        printf(WHITE("[")RED("ERROR")WHITE("]\t[%s] "), time_buf);
        break;
    case INFO:
        printf(WHITE("[")CYAN("INFO")WHITE("]\t[%s] "), time_buf);
        break;
    
    default:
        break;
    }
}

/**
 * @brief Core logging function that writes to console and file.
 * 
 * Handles formatted output with timestamp to both console (colored)
 * and log file (plain text). Thread-safe for file writes.
 * 
 * @param type The type of message (DEBUG, INFO, or ERROR).
 * @param fmt Format string (printf-style).
 * @param args Variable argument list.
 */
void log_cf(print_type type, const char *fmt, va_list args) {
    if (!fmt) return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_buf[32];
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", t);

    /* Print colored output to console */
    print_console(type);

    /* Copy args since we need to use them twice (console + file) */
    va_list args_copy;
    va_copy(args_copy, args);

    vprintf(fmt, args);
    printf("\n");

    /* Write to log file with mutex protection */
    if (log_file) {
        pthread_mutex_lock(&log_mutex);
        fprintf(log_file, "[%s] ", time_buf);
        vfprintf(log_file, fmt, args_copy);
        fprintf(log_file, "\n");
        fflush(log_file);
        pthread_mutex_unlock(&log_mutex);
    }

    va_end(args_copy);
}

/**
 * @brief Prints an optional message in yellow.
 * 
 * @param optional_msg Message to print, or NULL for just a newline.
 */
void print_msg(const char *optional_msg){
    if(optional_msg == NULL){
        printf("\n");
        return;
    }
    printf(YELLOW("%s\n"), optional_msg);
}

void debug_log(const char *fmt, ...) {
    if (!fmt) return;

    va_list args;
    va_start(args, fmt);
    log_cf(DEBUG, fmt, args);
    va_end(args);
}

void info_log(const char *fmt, ...) {
    if (!fmt) return;

    va_list args;
    va_start(args, fmt);
    log_cf(INFO, fmt, args);
    va_end(args);
}

void error_log(const char *fmt, ...) {
    if (!fmt) return;

    va_list args;
    va_start(args, fmt);
    log_cf(ERROR, fmt, args);
    va_end(args);
}

void throw_error(errors error_code, const char *optional_string){
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_buf[32];
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", t);

    print_console(ERROR);

    /* Convert error code to string */
    char *_to_write;
    switch (error_code)
    {
    case MEMORY_ALLOCATION:
        _to_write = "MEMORY_ALLOCATION. ";
        break;
    case NULL_VARIABLE:
        _to_write = "NULL_VARIABLE. ";
        break;
    case OUT_OF_RANGE:
        _to_write = "OUT_OF_RANGE. ";
        break;
    case EMPTY_LIST:
        _to_write = "EMPTY_LIST. ";
        break;
    case NOT_FOUND:
        _to_write = "NOT_FOUND. ";
        break;
    case SOCKET:
        _to_write = "SOCKET. ";
        break;
    case BIND:
        _to_write = "BIND. ";
        break;
    case LISTEN:
        _to_write = "LISTEN. ";
        break;
    case DATABASE:
        _to_write = "DATABASE. ";
        break;
    case DATABASE_PREPARE:
        _to_write = "DATABASE_PREPARE. ";
        break;
    case DATABASE_EXEC:
        _to_write = "DATABASE_EXEC. ";
        break;
    case DB_QUERY:
        _to_write = "DB_QUERY. ";
        break;
    case JSON_PARSING:
        _to_write = "JSON_PARSING. ";
        break;
    case ENCODING_ERROR:
        _to_write = "ENCODING_ERROR. ";
        break;
    case THREAD_CREATION:
        _to_write = "THREAD_CREATION. ";
        break;
    case CLIENT_ATTACH:
        _to_write = "CLIENT_ATTACH. ";
        break;

    default:
        _to_write = "";
        break;
    }

    /* Print error to console */
    printf("%s", _to_write);
    print_msg(optional_string);
    
    /* Write to log file with mutex protection */
    if (log_file) {
        pthread_mutex_lock(&log_mutex);
        fprintf(log_file, "[%s] ", time_buf);
        fprintf(log_file, "%s %s\n", _to_write, optional_string ? optional_string : "");
        fflush(log_file);
        pthread_mutex_unlock(&log_mutex);
    }
}
