#pragma once

typedef struct node {
    void *data;
    struct node *next;
} node;

typedef struct list {
    node *head;
    node *tail;
    int size;
} chained_list;



#include <stdio.h>
#include <stdlib.h>

#include "../errors/error.h"
#include "chained_list.h"

chained_list *clist_init();

void clist_destroy(chained_list *l);

int clist_append(chained_list *l, void *data);

int clist_insert(chained_list *l, void *data, int index);

void *clist_pop(chained_list *l, int index);

void clist_clear(chained_list *l);

int clist_size(chained_list *l);

int clist_find(chained_list *l, void *data);

void *clist_get(chained_list *l, int index);

int clist_remove(chained_list *l, void *data);