#ifndef ORDERED_LIST_H
#define ORDERED_LIST_H

#include <stdbool.h>

#include "list.h"
#include "iterator.h"

/**
 * Move the iterator to the first position which has value not less than given
 * value. If such position does not exist, move to end.
 */
static inline void iter_ordered_find(
    BidirectionalIterator* iter, void* value, int cmp(void* v1, void* v2)) {
  while (!iter_is_end(iter) && cmp(iter_get(iter), value) < 0) {
    iter_next(iter);
  }
}

/**
 * Insert an element into ordered list only if there is no element equal to
 * it.
 */
static inline bool ordered_list_insert_unique(
    LinkedList* list, void* value, int cmp(void* v1, void* v2)) {
  BidirectionalIterator* iter = list_iterator(list);
  bool ret;
  iter_ordered_find(iter, value, cmp);
  if (iter_is_end(iter) || cmp(value, iter_get(iter)) != 0) {
    iter_insert(iter, value);
    ret = true;
  }
  else
    ret = false;
  iter_destroy(iter);
  return ret;
}

#endif
