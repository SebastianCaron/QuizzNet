#include "db.h"

void init_table_theme(server *s){
    
    // Init of the THEMES table
    SqliteResult *result = exec_query(s, 
        "CREATE TABLE IF NOT EXISTS themes"
        "("
            "id INT PRIMARY KEY NOT NULL,"
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
            "id INT PRIMARY KEY NOT NULL,"
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
            "id INT PRIMARY KEY NOT NULL,"
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
