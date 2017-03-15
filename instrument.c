#include <stdlib.h>
#include <string.h>

#include "instrument.h"
#include "list.h"

LinkedList* INSTRUMENTS;

Instrument* instrument_create_default() {
  Instrument* ret = malloc(sizeof(Instrument));
  Parameters1Osc* params = malloc(sizeof(Parameters1Osc));
  
  strcpy(ret->identifier, "DEFLT");
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
}

void* instrument_destroy(Instrument* instrument) {
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
