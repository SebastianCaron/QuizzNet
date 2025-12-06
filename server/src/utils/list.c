#include "list.h"

list *list_init(){
    list *l = calloc(1, sizeof(list));

    if(l == NULL){
        throw_error(MEMORY_ALLOCATION, "Erreur allocation pour la structure liste");
        return NULL;
    }

    l->size = 0;
    l->capacity = 0;

    return l;
}

void list_destroy(list *l){
    if(!l) return;
    free(l->list);
    free(l);
}

int list_append(list *l, void *elt){
    if(!l) return 0;

    if(l->size >= l->capacity){
        int new_capacity = l->capacity + LIST_BLOCK_SIZE;
        l->list = realloc(l->list,  new_capacity * sizeof(void *));
        if(l->list == NULL){
            throw_error(MEMORY_ALLOCATION, "Erreur reallocation pour la structure liste");
            return 0;
        }
        l->capacity = new_capacity;
    }

    l->list[l->size] = elt;
    l->size++;
    return 1;
}

int list_insert(list *l, void *elt, int index){
    if(!l) return 0;

    if(index < 0 || index > l->size){
        throw_error(OUT_OF_RANGE, "Index hors limites dans list_insert");
        return 0;
    }

    if(l->size >= l->capacity){
        int new_capacity = l->capacity + LIST_BLOCK_SIZE;
        l->list = realloc(l->list, new_capacity * sizeof(void *));
        if(l->list == NULL){
            throw_error(MEMORY_ALLOCATION, "Erreur reallocation pour list_insert");
            return 0;
        }
        l->capacity = new_capacity;
    }

    for(int i = l->size; i > index; i--){
        l->list[i] = l->list[i - 1];
    }

    l->list[index] = elt;
    l->size++;
    return 1;
}

void *list_pop(list *l, int index){
    if(!l) return NULL;

    if(l->size == 0){
        throw_error(EMPTY_LIST, "Tentative de pop sur une liste vide");
        return NULL;
    }
    if(index < 0 || index >= l->size){
        throw_error(OUT_OF_RANGE, "Index hors limites dans list_pop");
        return NULL;
    }

    void *removed = l->list[index];

    for(int i = index; i < l->size - 1; i++){
        l->list[i] = l->list[i + 1];
    }

    l->size--;
    return removed;
}

void list_clear(list *l){
    if(l == NULL) return;
    free(l->list);
    l->list = NULL;
    l->size = 0;
    l->capacity = 0;
}

int list_size(list *l){
    if(l == NULL) return 0;
    return l->size;
}
