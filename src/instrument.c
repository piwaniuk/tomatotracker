#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "instrument.h"
#include "list.h"

LinkedList* INSTRUMENTS;

Instrument* instrument_create(const char* name, uint16_t type) {
  Instrument* ret = malloc(sizeof(Instrument));
  snprintf(ret->identifier, 7, "%s", name);
  ret->description[0] = '\0';
  ret->volume = 64;
  
  ret->type = type;
  switch (ret->type) {
  case INSTRUMENT_TYPE_1OSC:
    ret->parameters = malloc(sizeof(Parameters1Osc));
    *((Parameters1Osc*)ret->parameters) = (Parameters1Osc){
      SHAPE_SAW,
      240,
      240,
      40000,
      800,
      20000,
      0
    };
    break;
  default:
    ret->parameters = NULL;
  }
  return ret;
}

Instrument* instrument_create_default(void) {
  return instrument_create("deflt", INSTRUMENT_TYPE_DEFAULT);
}

void instrument_destroy(Instrument* instrument) {
  free(instrument->parameters);
  free(instrument);
}

int instrument_cmp_name(void* v1, void* v2) {
  Instrument* i1 = (Instrument*)v1;
  Instrument* i2 = (Instrument*)v2;
  return strcmp(i1->identifier, i2->identifier);
}

char* instrument_repr(void* data) {
  //return "foobar";
  Instrument* instrument = (Instrument*)data;
  return instrument->identifier;
}
