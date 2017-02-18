#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include <stdint.h>

#include "iterator.h"
#include "list.h"

extern LinkedList* INSTRUMENTS;

typedef enum {INSTRUMENT_1OSC} InstrType;

typedef struct {
  char identifier[7];
  char description[32];
  InstrType type;
  void* parameters;
} Instrument;

typedef struct {
  uint8_t shape;
  uint16_t amp;
  uint16_t ampAtt;
  uint16_t ampDec;
  uint16_t ampSus;
  uint16_t ampRel; 
} Parameters1Osc;

BidirectionalIterator* song_instruments(void* song);
char* instrument_repr(void*);

#endif
