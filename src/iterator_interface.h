#ifndef ITERATOR_INTERFACE_H
#define ITERATOR_INTERFACE_H

typedef struct IteratorInterface {
  char (*is_first)(struct BidirectionalIterator* i);
  char (*is_last)(struct BidirectionalIterator* i);
  char (*is_end)(struct BidirectionalIterator* i);
  
  void (*next)(struct BidirectionalIterator* i);
  void (*prev)(struct BidirectionalIterator* i);
  void* (*get)(struct BidirectionalIterator* i);
  
  void (*insert)(struct BidirectionalIterator* i, void* value);
  void* (*remove)(struct BidirectionalIterator* i);
  void (*destroy)(struct BidirectionalIterator* i);
} IteratorInterface;

typedef struct BidirectionalIterator {
  const IteratorInterface* interface;
  void* state;
} BidirectionalIterator;

#endif
