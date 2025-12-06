#include <string.h>

#include "../sqlite/sqlite3.h"
#include "../network/network.h"
#include "../errors/error.h"
#include "db.h"

void add_sqlite_to_server(server *s){
    /* Open or create the database file */
    int rc = sqlite3_open("test.db", &(s->db)); // sqlite3_open(":memory:", &db);

    if (rc != SQLITE_OK) {
        throw_error(DATABASE, sqlite3_errmsg((s->db)));
        sqlite3_close((s->db));
    }
}

SqliteResult *exec_query(server *s, char *sql) {
    SqliteResult *res = calloc(1, sizeof(SqliteResult));
    if(!res){
        throw_error(MEMORY_ALLOCATION, "Erreur allocation memoire dans exec_select");
        return NULL;
    }

    sqlite3_stmt *stmt;
    int rc;

    /* Prepare the SQL statement */
    rc = sqlite3_prepare_v2(s->db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        throw_error(DATABASE_PREPARE, sqlite3_errmsg(s->db));
        return res;
    }

    /* Extract column names */
    res->column_count = sqlite3_column_count(stmt);
    res->column_names = calloc(res->column_count, sizeof(char *));

    for (int i = 0; i < res->column_count; i++) {
        const char *name = sqlite3_column_name(stmt, i);
        res->column_names[i] = strdup(name ? name : "");
    }

    /* Allocate initial row storage */
    int capacity = SQLITE_RESULT_BLOCK_SIZE;
    res->rows = calloc(capacity, sizeof(char **));
    if(!res->rows){
        throw_error(MEMORY_ALLOCATION, "Erreur allocation memoire pour les lignes dans exec_select");
        res->row_count = 0;
        return res;
    }

    /* Fetch all rows */
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        /* Grow row array if needed */
        if (res->row_count >= capacity) {
            capacity += SQLITE_RESULT_BLOCK_SIZE;
            res->rows = realloc(res->rows, capacity * sizeof(char **));
        }

        /* Allocate and populate row data */
        char **row = calloc(res->column_count, sizeof(char *));
        if(!row){
            throw_error(MEMORY_ALLOCATION, "Erreur allocation memoire pour une ligne dans exec_select");
            break;
        }
        for (int i = 0; i < res->column_count; i++) {
            const unsigned char *text = sqlite3_column_text(stmt, i);
            row[i] = strdup((const char *)text ? (const char *)text : "");
        }
        res->rows[res->row_count++] = row;
    }

    if (rc != SQLITE_DONE) {
        throw_error(DATABASE_EXEC, sqlite3_errmsg(s->db));
    }

    sqlite3_finalize(stmt);
    return res;
}

void sqlite_result_destroy(SqliteResult *res){
    if(!res) return;
    
    /* Free all row data */
    for(int i = 0; i < res->row_count; i++){
        for(int j = 0; j < res->column_count; j++){
            free(res->rows[i][j]);
        }
        free(res->rows[i]);
    }

    /* Free column names */
    for(int i = 0; i < res->column_count; i++){
        free(res->column_names[i]);
    }
    free(res->column_names);
    free(res->rows);

    free(res);
}

void close_db(server *s){
    sqlite3_close(s->db);
}
