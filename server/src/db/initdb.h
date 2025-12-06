#pragma once

#include "db.h"

/**
 * @file initdb.h
 * @brief Database initialization and schema setup.
 * 
 * This module handles creating database tables and loading
 * initial content if the database is empty.
 * 
 * @authors
 * Sebastian Caron
 * Lina Lethoor
 */

/**
 * @brief Initializes the database schema and loads default content.
 * 
 * Creates all required tables (themes, clients, questions, questions_in_themes)
 * if they don't exist, and loads initial content if the database is empty.
 * 
 * @param s Pointer to the server structure.
 */
void init_db(server *s);
