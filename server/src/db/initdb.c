#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "db.h"
#include "../errors/error.h"

void init_table_theme(server *s){
    
    // Init of the THEMES table
    SqliteResult *result = exec_query(s, 
        "CREATE TABLE IF NOT EXISTS themes"
        "("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name VARCHAR(100) NOT NULL"
        ")"
    );

    sqlite_result_destroy(result);

}

void init_table_clients(server *s){
    
    // Init of the CLIENTS table
    SqliteResult *result = exec_query(s, 
        "CREATE TABLE IF NOT EXISTS clients"
        "("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "pseudo VARCHAR(100) NOT NULL,"
            "password VARCHAR(255) NOT NULL"
        ")"
    );

    sqlite_result_destroy(result);
}



void init_table_questions(server *s){
    
    // Init of the QUESTIONS table
    SqliteResult *result = exec_query(s,
        "CREATE TABLE IF NOT EXISTS questions"
        "("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "difficulty INT NOT NULL,"
            "type INT NOT NULL,"
            "statement VARCHAR(500),"
            "answers VARCHAR(500)"
        ")"
    );

    sqlite_result_destroy(result);

}

void init_table_theme_quest(server *s){
    
    // Init of the THEME-QUESTIONS table
    SqliteResult *result = exec_query(s,
        "CREATE TABLE IF NOT EXISTS question_in_theme"
        "("
            "id_question INT NOT NULL,"
            "id_theme INT NOT NULL,"
            "PRIMARY KEY(id_question, id_theme)"
        ")"
    );

    sqlite_result_destroy(result);

}

void init_db(server *s){
    debug_log("INITIALISATION DE LA DB.");
    init_table_theme(s);
    init_table_clients(s);
    init_table_questions(s);
    init_table_theme_quest(s);
    debug_log("DB OK!");
}

char *read_file_content(const char *filepath, long *file_size) {
    FILE *file = fopen(filepath, "r");
    if(!file) {
        throw_error(DATABASE, "Impossible d'ouvrir le fichier SQL");
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    *file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if(*file_size <= 0) {
        fclose(file);
        return NULL;
    }
    
    char *content = malloc(*file_size + 1);
    if(!content) {
        throw_error(MEMORY_ALLOCATION, "Erreur allocation pour le contenu du fichier SQL");
        fclose(file);
        return NULL;
    }
    
    size_t read_size = fread(content, 1, *file_size, file);
    content[read_size] = '\0';
    fclose(file);
    
    return content;
}

char *trim_query(char *query) {
    if(!query) return NULL;
    
    char *start = query;
    while(*start == ' ' || *start == '\t' || *start == '\n' || *start == '\r') {
        start++;
    }
    
    char *end = start + strlen(start) - 1;
    while(end > start && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r' || *end == ';')) {
        *end = '\0';
        end--;
    }
    
    return start;
}

int execute_single_query(server *s, const char *query, int line_num) {
    if(!query || strlen(query) == 0) {
        return 0;
    }
    
    SqliteResult *result = exec_query(s, query);
    if(result) {
        sqlite_result_destroy(result);
        return 1;
    } else {
        error_log("Erreur lors de l'exécution de la requête à la ligne %d", line_num);
        return -1;
    }
}

char *skip_comment(char *current, int *line_num) {
    if(*current == '-' && *(current + 1) == '-') {
        while(*current != '\0' && *current != '\n') {
            current++;
        }
        if(*current == '\n') {
            (*line_num)++;
            current++;
        }
    }
    return current;
}

int parse_and_execute_queries(server *s, char *content) {
    char *current = content;
    char *query_start = NULL;
    int in_query = 0;
    int line_num = 1;
    int query_count = 0;
    int error_count = 0;
    
    while(*current != '\0') {
        char *after_comment = skip_comment(current, &line_num);
        if(after_comment != current) {
            current = after_comment;
            continue;
        }
        
        if(!in_query) {
            if(*current == ' ' || *current == '\t' || *current == '\n' || *current == '\r') {
                if(*current == '\n') line_num++;
                current++;
                continue;
            }
            query_start = current;
            in_query = 1;
        }
        
        if(*current == ';') {
            size_t query_len = current - query_start + 1;
            char *query = malloc(query_len + 1);
            if(!query) {
                throw_error(MEMORY_ALLOCATION, "Erreur allocation pour une requête SQL");
                current++;
                continue;
            }
            
            strncpy(query, query_start, query_len);
            query[query_len] = '\0';
            
            char *trimmed = trim_query(query);
            int result = execute_single_query(s, trimmed, line_num);
            if(result > 0) {
                query_count++;
            } else if(result < 0) {
                error_count++;
            }
            
            free(query);
            in_query = 0;
            current++;
            if(*current == '\n') {
                line_num++;
                current++;
            }
        } else {
            if(*current == '\n') {
                line_num++;
            }
            current++;
        }
    }
    
    if(in_query && query_start) {
        size_t query_len = current - query_start;
        if(query_len > 0) {
            char *query = malloc(query_len + 1);
            if(query) {
                strncpy(query, query_start, query_len);
                query[query_len] = '\0';
                
                char *trimmed = trim_query(query);
                int result = execute_single_query(s, trimmed, line_num);
                if(result > 0) {
                    query_count++;
                } else if(result < 0) {
                    error_count++;
                }
                
                free(query);
            }
        }
    }
    
    if(error_count > 0) {
        error_log("exec_sql_file: %d requêtes exécutées avec succès, %d erreurs", query_count, error_count);
        return -1;
    }
    
    return query_count;
}

int exec_sql_file(server *s, const char *filepath) {
    if(!s || !filepath) {
        throw_error(NULL_VARIABLE, "exec_sql_file: paramètres NULL");
        return -1;
    }
    
    long file_size = 0;
    char *content = read_file_content(filepath, &file_size);
    if(!content) {
        return (file_size == 0) ? 0 : -1;
    }
    
    int query_count = parse_and_execute_queries(s, content);
    
    free(content);
    
    if(query_count >= 0) {
        debug_log("exec_sql_file: %d requêtes exécutées avec succès depuis %s", query_count, filepath);
    }
    
    return query_count;
}