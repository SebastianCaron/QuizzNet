#pragma once

#include "../network/network.h"

#define SQLITE_RESULT_BLOCK_SIZE 256

typedef struct {
    int column_count;
    int row_count;
    char **column_names;
    char ***rows;
} SqliteResult;



void sqlite_result_destroy(SqliteResult *res);

SqliteResult *exec_query(server *s, char *sql);

void close_db(server *s);

void add_sqlite_to_server(server *s);