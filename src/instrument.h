#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include <stdint.h>

#include "iterator.h"
#include "list.h"
#include "1osc.h"

extern LinkedList* INSTRUMENTS;

enum InstrumentType {
  INSTRUMENT_TYPE_NONE = 0,
  INSTRUMENT_TYPE_1OSC,
  INSTRUMENT_TYPE_SAMPLER
};

#define LAST_INSTRUMENT_TYPE 2
#define INSTRUMENT_TYPE_DEFAULT INSTRUMENT_TYPE_1OSC

typedef struct {
  char identifier[7];
  char description[32];
  uint8_t volume;
  uint16_t type;
  void* parameters;
} Instrument;


Instrument* instrument_create(const char* name, uint16_t type);
Instrument* instrument_create_default(void);
void instrument_destroy(Instrument* instrument);
int instrument_cmp_name(void* v1, void* v2);
char* instrument_repr(void*);

#endif
