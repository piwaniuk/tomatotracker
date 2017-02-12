#include <stdlib.h>

#include "list.h"
#include "iterator_interface.h"

typedef struct ListEntry {
  struct ListEntry* next;
  struct ListEntry* prev;
  void* value;
} ListEntry;

struct LinkedList {
  ListEntry* head;
};

char list_is_first(BidirectionalIterator* iter) {
  ListEntry* entry = (ListEntry*)iter->state;
  return entry->prev == NULL;
}

char list_is_last(BidirectionalIterator* iter) {
  ListEntry* entry = (ListEntry*)iter->state;
  return entry->next->next == NULL;
}

char list_is_end(BidirectionalIterator* iter) {
  ListEntry* entry = (ListEntry*)iter->state;
  return entry->next == NULL;
}

void list_next(BidirectionalIterator* iter) {
  ListEntry* entry = (ListEntry*)iter->state;
  iter->state = entry->next;
}

void list_prev(BidirectionalIterator* iter) {
  ListEntry* entry = (ListEntry*)iter->state;
  iter->state = entry->prev;
}

void* list_get(BidirectionalIterator* iter) {
  ListEntry* entry = (ListEntry*)iter->state;
  return entry->next->value;
}

void list_insert(BidirectionalIterator* iter, void* value) {
  ListEntry* entry = (ListEntry*)iter->state;
  ListEntry* newEntry = malloc(sizeof(ListEntry));
  newEntry->value = value;
  newEntry->next = entry->next;
  newEntry->prev = entry;
  newEntry->prev->next = newEntry;
  if (newEntry->next)
  newEntry->next->prev = newEntry;
  // move forward
  iter->state = newEntry;
}

void* list_remove(BidirectionalIterator* iter) {
  ListEntry* entry = (ListEntry*)iter->state;
  ListEntry* oldEntry = entry->next;
  entry->next = entry->next->next;
  if (entry->next)
    entry->next->prev = entry;
  void* ret = oldEntry->value;
  free(oldEntry);
  return ret;
}

void list_iterator_destroy(BidirectionalIterator* iter) {
  free(iter);
}

const IteratorInterface LIST_ITERATOR_INTERFACE = {
  list_is_first, list_is_last, list_is_end,
  list_next, list_prev, list_get,
  list_insert, list_remove, list_iterator_destroy
};

BidirectionalIterator* list_iterator(LinkedList* list) {
  BidirectionalIterator* ret = malloc(sizeof(BidirectionalIterator));
  ret->interface = &LIST_ITERATOR_INTERFACE;
  ret->state = list->head;
  return ret;
}

LinkedList* list_create(void) {
  LinkedList* list = malloc(sizeof(LinkedList));
  ListEntry* guard = malloc(sizeof(ListEntry));
  guard->next = NULL;
  guard->prev = NULL;
  guard->value = NULL;
  list->head = guard;
  return list;
}

void list_destroy(LinkedList* list) {
  while (list->head != NULL) {
    ListEntry* oldHead = list->head;
    list->head = list->head->next;
    free(oldHead);
  }
  free(list);
}

