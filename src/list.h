#ifndef LIST_H
#define LIST_H

#include <stdlib.h>

#include "iterator.h"

struct LinkedList;

typedef struct LinkedList LinkedList;

LinkedList* list_create(void);

void list_destroy(LinkedList* list);

BidirectionalIterator* list_iterator(LinkedList* list);

size_t list_length(LinkedList* list);

#endif
