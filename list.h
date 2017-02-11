#ifndef LIST_H
#define LIST_H

#include "iterator.h"

struct LinkedList;

typedef struct LinkedList LinkedList;

LinkedList* list_create();

void list_destroy(LinkedList* list);

BidirectionalIterator* list_iterator(LinkedList* list);


#endif
