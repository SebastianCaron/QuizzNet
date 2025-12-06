#pragma once

/**
 * @file hash_pass.h
 * @brief Password hashing utilities.
 * 
 * This module provides secure password hashing using SHA-256
 * with base64 encoding for storage.
 * 
 * @authors
 * Sebastian Caron
 * Lina Lethoor
 */

/**
 * @brief Hashes a password using SHA-256 and encodes it in base64.
 * 
 * Computes the SHA-256 hash of the password string, then encodes
 * the result in base64 for safe storage in the database.
 * 
 * @param password The plain text password to hash.
 * @return Newly allocated base64-encoded hash string, or NULL on failure.
 * @note The caller is responsible for freeing the returned string.
 */
unsigned char* hash_password(const char* password);
