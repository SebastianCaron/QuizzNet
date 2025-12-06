#pragma once

#include <stdlib.h>

#include "../errors/error.h"

/**
 * @file list.h
 * @brief Dynamic array list implementation.
 * 
 * This data structure allows storing pointers to any type of data (void*)
 * using a dynamically resizing array.
 * 
 * @authors
 * Sebastian Caron
 * Lina Lethoor
 */

/**
 * @def LIST_BLOCK_SIZE
 * @brief Number of elements to allocate when the list needs to grow.
 */
#define LIST_BLOCK_SIZE 256

/**
 * @struct list
 * @brief Represents a dynamic array list.
 * 
 * @param list Pointer to the array of void pointers.
 * @param size Current number of elements in the list.
 * @param capacity Current allocated capacity of the list.
 */
typedef struct {
    void **list;
    long size;
    long capacity;
} list;

/**
 * @brief Initializes a new empty dynamic list.
 * 
 * Allocates memory for a new list structure with zero size and capacity.
 * 
 * @return Pointer to the created list, or NULL on allocation failure.
 */
list *list_init();

/**
 * @brief Frees the memory occupied by the list.
 * 
 * Frees the internal array and the list structure itself.
 * Note: The data pointed to by each element is NOT freed.
 * 
 * @param l Pointer to the list to destroy.
 */
void list_destroy(list *l);

/**
 * @brief Appends an element to the end of the list.
 * 
 * Automatically grows the list capacity if needed.
 * 
 * @param l Pointer to the list.
 * @param elt Pointer to the element to add.
 * @return 1 on success, 0 on failure.
 */
int list_append(list *l, void *elt);

/**
 * @brief Inserts an element at a given position in the list.
 * 
 * Shifts all elements after the index to make room.
 * Automatically grows the list capacity if needed.
 * 
 * @param l Pointer to the list.
 * @param elt Pointer to the element to insert.
 * @param index Position where to insert the element (0 = beginning).
 * @return 1 on success, 0 on failure (out of bounds index or allocation error).
 */
int list_insert(list *l, void *elt, int index);

/**
 * @brief Removes and returns the element at the given position.
 * 
 * Shifts all elements after the index to fill the gap.
 * The returned data must be managed by the caller.
 * 
 * @param l Pointer to the list.
 * @param index Position of the element to remove.
 * @return Pointer to the removed element, or NULL if index is invalid or list is empty.
 */
void *list_pop(list *l, int index);

/**
 * @brief Clears the list by removing all its elements.
 * 
 * Frees the internal array but not the pointed data.
 * The list remains usable after this operation (empty with zero capacity).
 * 
 * @param l Pointer to the list to clear.
 */
void list_clear(list *l);

/**
 * @brief Returns the number of elements in the list.
 * 
 * @param l Pointer to the list.
 * @return Number of elements, or 0 if the list is NULL.
 */
int list_size(list *l);
