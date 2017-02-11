#include "iterator.h"
#include "iterator_interface.h"

char iter_is_first(BidirectionalIterator* i) {
  return i->interface->is_first(i);
}

char iter_is_last(BidirectionalIterator* i) {
  return i->interface->is_last(i);
}

char iter_is_end(BidirectionalIterator* i) {
  return i->interface->is_end(i);
}

void iter_next(BidirectionalIterator* i) {
  i->interface->next(i);
}

void iter_prev(BidirectionalIterator* i) {
  i->interface->prev(i);
}

void* iter_get(BidirectionalIterator* i) {
  return i->interface->get(i);
}

void iter_insert(BidirectionalIterator* i, void* value) {
  i->interface->insert(i, value);
}

void* iter_remove(BidirectionalIterator* i) {
  return i->interface->remove(i);
}

void iter_destroy(BidirectionalIterator* i) {
  i->interface->destroy(i);
}

void iter_find_forward(BidirectionalIterator* i, void* value) {
  while (!iter_is_end(i)) {
    if (iter_get(i) == value)
      break;
    iter_next(i);
  }
}

