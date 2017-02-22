#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include <stdint.h>

#include "iterator.h"
#include "list.h"
#include "1osc.h"

extern LinkedList* INSTRUMENTS;

typedef enum {INSTRUMENT_1OSC} InstrType;

typedef struct {
  char identifier[7];
  char description[32];
  InstrType type;
  void* parameters;
} Instrument;

BidirectionalIterator* song_instruments(void* song);
char* instrument_repr(void*);

#endif
