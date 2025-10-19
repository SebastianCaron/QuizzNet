#pragma once

#define LIST_BLOCK_SIZE 256

typedef struct {
    void **list;
    long size;
    long capacity;
} list;

list *list_init();

void list_destroy(list *);

int list_append(list *l, void *elt);

int list_insert(list *l, void *elt, int index);

void *list_pop(list *l, int index);

void list_clear(list *l);

int list_size(list *l);