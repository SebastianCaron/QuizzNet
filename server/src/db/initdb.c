#include "db.h"
#include "sql_content.h"
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
            "answers VARCHAR(500),"
            "explanation VARCHAR(500)"
        ")"
    );

    sqlite_result_destroy(result);

}

void init_table_theme_quest(server *s){
    
    // Init of the THEME-QUESTIONS table
    SqliteResult *result = exec_query(s,
        "CREATE TABLE IF NOT EXISTS questions_in_themes"
        "("
            "id_question INT NOT NULL,"
            "id_theme INT NOT NULL,"
            "PRIMARY KEY(id_question, id_theme)"
        ")"
    );

    sqlite_result_destroy(result);

}

void load_content(server *s){
    char query[4096] = {'\0'};

    char *all_query = SQL_CONTENT;

    int i = 0;
    int j = 0;
    int nb = 0;
    SqliteResult *result;
    while(all_query[i] != '\0'){
        if(j >= 4095){
            throw_error(DATABASE_PREPARE, "Query too long (max 4096 characters)");
            break;
        }
        query[j] = all_query[i];
        j++;
        if(query[j-1] == ';'){
            query[j] = '\0';
            // debug_log("Query %d: %s\n",nb, query);
            result = exec_query(s, query);
            sqlite_result_destroy(result);
            j = 0;
            nb++;
        }
        i++;
    }

}

void init_db(server *s){
    debug_log("INITIALISATION DE LA DB.");
    init_table_theme(s);
    init_table_clients(s);
    init_table_questions(s);
    init_table_theme_quest(s);
    load_content(s);
    debug_log("DB OK!");
}
