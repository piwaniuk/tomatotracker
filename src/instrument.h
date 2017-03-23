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


Instrument* instrument_create(const char* name);
Instrument* instrument_create_default(void);
void instrument_destroy(Instrument* instrument);
int instrument_cmp_name(void* v1, void* v2);
char* instrument_repr(void*);

#endif
