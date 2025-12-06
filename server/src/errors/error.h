#pragma once

/**
 * @file error.h
 * @brief Error handling and logging utilities.
 * 
 * This module provides error codes, colored console output macros,
 * and logging functions for debug, info, and error messages.
 * Logs are written both to the console (with colors) and to a timestamped file.
 * 
 * @authors
 * Sebastian Caron
 * Lina Lethoor
 */

/**
 * @enum errors
 * @brief Error codes used throughout the application.
 */
typedef enum {
    NULL_VARIABLE,      /**< Unexpected NULL pointer encountered */
    MEMORY_ALLOCATION,  /**< Memory allocation failure (malloc/calloc/realloc) */
    OUT_OF_RANGE,       /**< Index out of bounds */
    EMPTY_LIST,         /**< Operation on an empty list */
    NOT_FOUND,          /**< Requested element not found */
    SOCKET,             /**< Socket creation error */
    BIND,               /**< Socket bind error */
    LISTEN,             /**< Socket listen error */
    DATABASE,           /**< General database error */
    DATABASE_PREPARE,   /**< SQLite statement preparation error */
    DATABASE_EXEC,      /**< SQLite execution error */
    DB_QUERY,           /**< Database query error */
    JSON_PARSING,       /**< JSON parsing error */
    ENCODING_ERROR,     /**< Character encoding error */
    THREAD_CREATION,    /**< Thread creation failure */
    CLIENT_ATTACH       /**< Failed to attach client to server */
} errors;

/**
 * @defgroup ColorMacros Console Color Macros
 * @brief ANSI escape codes for colored console output.
 * @{
 */
#define RED(m) "\033[01;31m"m"\033[0m"      /**< Red text (errors) */
#define GREEN(m) "\033[01;32m"m"\033[0m"    /**< Green text (success) */
#define YELLOW(m) "\033[01;33m"m"\033[0m"   /**< Yellow text (warnings/debug) */
#define BLUE(m) "\033[01;34m"m"\033[0m"     /**< Blue text */
#define MAGENTA(m) "\033[01;35m"m"\033[0m"  /**< Magenta text */
#define CYAN(m) "\033[01;36m"m"\033[0m"     /**< Cyan text (info) */
#define WHITE(m) "\033[01;37m"m"\033[0m"    /**< White text */
/** @} */

/**
 * @brief Logs an error with its code and optional message.
 * 
 * Prints to console with timestamp and ERROR tag, and writes to log file.
 * 
 * @param error_code The error type from the errors enum.
 * @param optional_msg Optional additional message (can be NULL).
 */
void throw_error(errors error_code, const char *optional_msg);

/**
 * @brief Logs a debug message.
 * 
 * Prints to console with DEBUG tag and writes to log file.
 * Supports printf-style formatting.
 * 
 * @param fmt Format string (printf-style).
 * @param ... Variable arguments for the format string.
 */
void debug_log(const char *fmt, ...);

/**
 * @brief Logs an informational message.
 * 
 * Prints to console with INFO tag and writes to log file.
 * Supports printf-style formatting.
 * 
 * @param fmt Format string (printf-style).
 * @param ... Variable arguments for the format string.
 */
void info_log(const char *fmt, ...);

/**
 * @brief Initializes the log file for the session.
 * 
 * Creates a new log file with timestamp in the filename (YYYY_MM_DD_HH_MM_SS.log).
 * 
 * @return 0 on success, -1 on failure.
 */
int init_debug_log(void);

/**
 * @brief Closes the log file.
 * 
 * Should be called before program exit to flush and close the log file.
 */
void close_debug_log(void);
