#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "instrument.h"
#include "list.h"

LinkedList* INSTRUMENTS;

Instrument* instrument_create(const char* name) {
  Instrument* ret = malloc(sizeof(Instrument));
  Parameters1Osc* params = malloc(sizeof(Parameters1Osc));
  
  snprintf(ret->identifier, 7, "%s", name);
  ret->description[0] = '\0';
  ret->type = INSTRUMENT_1OSC;
  ret->parameters = params;
  
  *params = (Parameters1Osc){
    0,
    65535,
    240,
    240,
    40000,
    800,
  };
  return ret;
}

Instrument* instrument_create_default(void) {
  return instrument_create("DEFLT");
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
