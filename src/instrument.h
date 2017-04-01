#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include <stdint.h>

#include "iterator.h"
#include "list.h"
#include "1osc.h"

extern LinkedList* INSTRUMENTS;

enum {INSTRUMENT_TYPE_1OSC = 1};

typedef struct {
  char identifier[7];
  char description[32];
  uint8_t volume;
  uint16_t type;
  void* parameters;
} Instrument;


Instrument* instrument_create(const char* name);
Instrument* instrument_create_default(void);
void instrument_destroy(Instrument* instrument);
int instrument_cmp_name(void* v1, void* v2);
char* instrument_repr(void*);

#endif
