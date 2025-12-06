#pragma once

#include "../network/network.h"

/**
 * @file db.h
 * @brief SQLite database wrapper and query execution.
 * 
 * This module provides a simplified interface for executing SQL queries
 * and managing the SQLite database connection.
 * 
 * @authors
 * Sebastian Caron
 * Lina Lethoor
 */

/**
 * @def SQLITE_RESULT_BLOCK_SIZE
 * @brief Number of rows to allocate at once when growing result arrays.
 */
#define SQLITE_RESULT_BLOCK_SIZE 256

/**
 * @struct SqliteResult
 * @brief Stores the result of a SQL query.
 * 
 * @param column_count Number of columns in the result.
 * @param row_count Number of rows returned.
 * @param column_names Array of column name strings.
 * @param rows 2D array of row data (rows[row_index][column_index]).
 */
typedef struct {
    int column_count;
    int row_count;
    char **column_names;
    char ***rows;
} SqliteResult;

/**
 * @brief Frees all memory associated with a query result.
 * 
 * @param res Pointer to the SqliteResult to destroy.
 */
void sqlite_result_destroy(SqliteResult *res);

/**
 * @brief Executes a SQL query and returns the result.
 * 
 * Supports SELECT, INSERT, UPDATE, DELETE and other SQL statements.
 * For SELECT queries, returns all rows and column names.
 * 
 * @param s Pointer to the server (contains db handle).
 * @param sql SQL query string to execute.
 * @return Pointer to SqliteResult, or NULL on error.
 * @note The caller must free the result with sqlite_result_destroy().
 */
SqliteResult *exec_query(server *s, char *sql);

/**
 * @brief Closes the SQLite database connection.
 * 
 * @param s Pointer to the server.
 */
void close_db(server *s);

/**
 * @brief Opens the SQLite database and attaches it to the server.
 * 
 * Opens or creates the "test.db" database file.
 * 
 * @param s Pointer to the server structure.
 */
void add_sqlite_to_server(server *s);
