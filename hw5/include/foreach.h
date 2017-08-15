#ifndef FOREACH_H
#define FOREACH_H

#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "const.h"

#define foreach(type, value, items) for ( type *value = foreach_init(items)  \
                                        ; value ;                            \
                                        value = foreach_next(items, value))

#define foreach_break if( foreach_break_f() ) break

typedef struct{
    int currentIndex;
    arraylist_t* list;
}local_data_t;

void *foreach_init(arraylist_t *self);

void *foreach_next(arraylist_t *self, void* data);

size_t foreach_index();

bool foreach_break_f();

int apply(arraylist_t *self, int (*application)(void*));

void create_key_once();

#endif
