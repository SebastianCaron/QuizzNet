#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "../errors/error.h"

/**
 * @file chained_list.h
 * @brief Generic linked list implementation.
 * 
 * This data structure allows storing pointers to any type of data (void*).
 * 
 * @authors
 * Sebastian Caron
 * Lina Lethoor
 */

/**
 * @struct node
 * @brief Represents a node in the linked list.
 * 
 * @param data Pointer to the data stored in the node.
 * @param next Pointer to the next node in the list.
 */
typedef struct node {
    void *data;
    struct node *next;
} node;

/**
 * @struct list
 * @brief Represents a linked list.
 * 
 * @param head Pointer to the first node in the list.
 * @param tail Pointer to the last node in the list.
 * @param size Number of elements in the list.
 */
typedef struct list {
    node *head;
    node *tail;
    int size;
} chained_list;

/**
 * @brief Initializes a new empty linked list.
 * 
 * Allocates memory for a new list and initializes its fields to NULL/0.
 * 
 * @return Pointer to the created list, or NULL on allocation failure.
 */
chained_list *clist_init();

/**
 * @brief Frees the memory occupied by the list.
 * 
 * Frees all nodes in the list as well as the list structure itself.
 * Note: The data pointed to by each node is NOT freed.
 * 
 * @param l Pointer to the list to destroy.
 */
void clist_destroy(chained_list *l);

/**
 * @brief Appends an element to the end of the list.
 * 
 * @param l Pointer to the list.
 * @param data Pointer to the data to add.
 * @return 1 on success, 0 on failure.
 */
int clist_append(chained_list *l, void *data);

/**
 * @brief Inserts an element at a given position in the list.
 * 
 * @param l Pointer to the list.
 * @param data Pointer to the data to insert.
 * @param index Position where to insert the element (0 = beginning).
 * @return 1 on success, 0 on failure (out of bounds index or allocation error).
 */
int clist_insert(chained_list *l, void *data, int index);

/**
 * @brief Removes and returns the element at the given position.
 * 
 * The node is freed but the pointed data is returned and
 * must be managed by the caller.
 * 
 * @param l Pointer to the list.
 * @param index Position of the element to remove.
 * @return Pointer to the data of the removed element, or NULL if index is invalid.
 */
void *clist_pop(chained_list *l, int index);

/**
 * @brief Clears the list by removing all its elements.
 * 
 * Frees all nodes but not the pointed data.
 * The list remains usable after this operation (empty).
 * 
 * @param l Pointer to the list to clear.
 */
void clist_clear(chained_list *l);

/**
 * @brief Returns the number of elements in the list.
 * 
 * @param l Pointer to the list.
 * @return Number of elements, or 0 if the list is NULL.
 */
int clist_size(chained_list *l);

/**
 * @brief Searches for an element in the list by pointer comparison.
 * 
 * @param l Pointer to the list.
 * @param data Pointer to the data to search for.
 * @return Index of the found element, or -1 if not found.
 */
int clist_find(chained_list *l, void *data);

/**
 * @brief Gets the element at the given position without removing it.
 * 
 * @param l Pointer to the list.
 * @param index Position of the element to retrieve.
 * @return Pointer to the element's data, or NULL if index is invalid.
 */
void *clist_get(chained_list *l, int index);

/**
 * @brief Removes an element from the list by pointer comparison.
 * 
 * Removes the first element whose data pointer matches.
 * The node is freed but the pointed data is not.
 * 
 * @param l Pointer to the list.
 * @param data Pointer to the data of the element to remove.
 * @return 1 if the element was found and removed, 0 otherwise.
 */
int clist_remove(chained_list *l, void *data);
