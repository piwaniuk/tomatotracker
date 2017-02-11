#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include <stdint.h>

#include "iterator.h"
#include "list.h"

extern LinkedList* INSTRUMENTS;

typedef struct {
  char identifier[7];
  char description[32];
  uint8_t waveform;
} Instrument;

BidirectionalIterator* song_instruments(void* song);
char* instrument_repr(void*);


#endif
