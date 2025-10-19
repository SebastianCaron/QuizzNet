#pragma once

#include "../network/network.h"

typedef struct {
    int column_count;
    int row_count;
    char **column_names;
    char ***rows;
} SqliteResult;


void free_result(SqliteResult *res);

SqliteResult *exec_select(server *s, char *sql);

void close_db(server *s);

void add_sqlite_to_server(server *s);