#include "chained_list.h"

chained_list *clist_init() {
    chained_list *l = calloc(1, sizeof(chained_list));
    if (l == NULL) {
        throw_error(MEMORY_ALLOCATION, "Erreur allocation pour la structure chained_liste");
        return NULL;
    }
    return l;
}

void clist_destroy(chained_list *l) {
    if (!l) return;

    node *current = l->head;
    while (current) {
        node *tmp = current;
        current = current->next;
        free(tmp);
    }

    free(l);
}

int clist_append(chained_list *l, void *data) {
    if (!l) return 0;

    node *n = malloc(sizeof(node));
    if (!n) {
        throw_error(MEMORY_ALLOCATION, "Erreur allocation pour un noeud de liste chainee");
        return 0;
    }
    n->data = data;
    n->next = NULL;

    if (l->tail)
        l->tail->next = n;
    else
        l->head = n;

    l->tail = n;
    l->size++;
    return 1;
}

int clist_insert(chained_list *l, void *data, int index) {
    if (!l || index < 0 || index > l->size){
        throw_error(OUT_OF_RANGE, "Index hors limites dans clist_insert");
        return 0;
    }

    node *n = malloc(sizeof(node));
    if (!n){
        throw_error(MEMORY_ALLOCATION, "Erreur allocation pour un noeud de liste chainee");
        return 0;
    }
    n->data = data;

    if (index == 0) {
        n->next = l->head;
        l->head = n;
        if (l->size == 0)
            l->tail = n;
    } else {
        node *prev = l->head;
        for (int i = 0; i < index - 1; i++)
            prev = prev->next;

        n->next = prev->next;
        prev->next = n;

        if (n->next == NULL)
            l->tail = n;
    }

    l->size++;
    return 1;
}

void *clist_pop(chained_list *l, int index) {
    if (!l || l->size == 0 || index < 0 || index >= l->size)
    {
        throw_error(OUT_OF_RANGE, "Index hors limites dans clist_pop");
        return NULL;
    }

    node *removed;
    void *data;

    if (index == 0) {
        removed = l->head;
        l->head = removed->next;
        if (l->head == NULL)
            l->tail = NULL;
    } else {
        node *prev = l->head;
        for (int i = 0; i < index - 1; i++)
            prev = prev->next;

        removed = prev->next;
        prev->next = removed->next;

        if (prev->next == NULL)
            l->tail = prev;
    }

    data = removed->data;
    free(removed);
    l->size--;
    return data;
}

void clist_clear(chained_list *l) {
    if (!l) return;

    node *current = l->head;
    while (current) {
        node *tmp = current;
        current = current->next;
        free(tmp);
    }

    l->head = NULL;
    l->tail = NULL;
    l->size = 0;
}

int clist_size(chained_list *l) {
    if (!l) return 0;
    return l->size;
}

int clist_find(chained_list *l, void *data) {
    if (!l) return -1;

    node *current = l->head;
    int index = 0;
    while (current) {
        if (current->data == data)
            return index;
        current = current->next;
        index++;
    }
    return -1;
}

void *clist_get(chained_list *l, int index){
    if(!l) return NULL;

    if(index < 0 || index >= l->size){
        throw_error(OUT_OF_RANGE, "Index hors limites dans clist_get");
        return NULL;
    }

    node *current = l->head;
    int i = 0;
    while (current) {
        if (index == i)
            return current->data;
        current = current->next;
        i++;
    }
    return NULL;
}

int clist_remove(chained_list *l, void *data){
    if(!l) return 0;

    node *current = l->head;
    node *prec = NULL;
    while (current) {
        if (current->data == data){
            if(prec){
                prec->next = current->next;
                free(current);
                l->size--;
                return 1;
            }
            l->head = NULL;
            free(current);
            l->size--;
            return 1;
        }
        prec = current;
        current = current->next;
    }
    return 0;
}