#ifndef ITERATOR_H
#define ITERATOR_H

struct BidirectionalIterator;

typedef struct BidirectionalIterator BidirectionalIterator;

// base methods

char iter_is_first(BidirectionalIterator* i);
char iter_is_last(BidirectionalIterator* i);
char iter_is_end(BidirectionalIterator* i);

void iter_next(BidirectionalIterator* i);
void iter_prev(BidirectionalIterator* i);
void* iter_get(BidirectionalIterator* i);

void iter_insert(BidirectionalIterator* i, void* value);
void* iter_remove(BidirectionalIterator* i);
void iter_destroy(BidirectionalIterator* i);

// more complex methods

void iter_find_forward(BidirectionalIterator* i, void* value);

#endif
